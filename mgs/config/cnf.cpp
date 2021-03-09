#include "cnf.h"

DataConfig::DataConfig(std::string filename) {
	this->filename = filename;
}

DataConfig::DataConfig(uint8_t* data, int size) {
	this->data = data;
	this->dataSize = size;
}

DataConfig::~DataConfig() {
}

void DataConfig::setWorkDir(std::string& workdir) {
	this->workdir = &workdir;
}

void DataConfig::setHandler(CNFHandler* cnfHandler) {
	this->cnfHandler = cnfHandler;
}

const char* DataConfig::getRegionForID(uint32_t id) {
	for (int i = 0; i < 7; i++) {
		if (cnfRegion[i].id == id)
			return cnfRegion[i].name;
	}
	return "";
}

void DataConfig::readVram(std::vector<std::string> tokens) {
	for (int i = 1; i < tokens.size(); i++) {
		if (cnfHandler) cnfHandler->processVram(tokens[i], currentRegion, workdir);
	}
}

uint32_t DataConfig::getCNFSize() {
	return sizeof(DataCNF) + sizeof(DataCNFTag) * cnf->numTags;
}

void DataConfig::readFace(std::vector<std::string> tokens) {}

void DataConfig::readFlag(std::string line) {
	std::stringstream lineStream(line);
	std::vector<std::string> tokens = { std::istream_iterator<std::string>{lineStream}, std::istream_iterator<std::string>{} };
	std::string flagName = tokens[0];

	if (flagName == "face") readFace(tokens); else
		if (flagName == "vram") readVram(tokens); else
			currentRegion = flagName;
}

void DataConfig::readFile(DataCNFTag tag, DataCNFTag nextTag) {
	uint32_t id = tag.id;
	int64_t offset = tag.offset;
	int64_t nextOffset = nextTag.offset;

	int size = nextOffset < offset ? 0 : nextOffset - offset;
	uint32_t fileext = id >> 24;
	uint32_t filehash = id & 0xFFFFFF;
	std::string filename = (FileNames[filehash] != "") ? FileNames[filehash] : intToHexString(filehash);
	filename = filename + "." + getExtForID(fileext);

	if (cnfHandler) cnfHandler->processFile(filename, currentRegion, workdir, &section[offset], size);
}

void DataConfig::readEnc(DataCNFTag tag) {
	currentEncSize = tag.id & 0xFFFFFF;
	int64_t offset = tag.offset;

	if (cnfHandler) cnfHandler->processEnc(currentRegion, &section, currentEncSize, currentSectionSize);
}

void DataConfig::readFace(DataCNFTag tag, DataCNFTag nextTag) {}

void DataConfig::startSection(DataCNFTag tag) {
	uint32_t flagID = tag.id & 0xFFFFFF;

	currentRegion = getRegionForID(flagID);
	currentSectionSize = tag.offset;
	dataPtr += cnfHandler->getNextSectorOffset(dataPtr);
	section = &data[dataPtr];
}

void DataConfig::endSection(DataCNFTag tag) {
	if (currentEncSize) {
		dataPtr += currentEncSize;
		currentEncSize = 0;
		delete[] section;
	}
	else {
		dataPtr += tag.offset;
	}
}

void DataConfig::readSection(DataCNFTag tag) {
	uint32_t flagID = tag.id & 0xFFFFFF;

	flagID ? startSection(tag) : endSection(tag);
}

void DataConfig::readFlag(DataCNFTag tag) {}

void DataConfig::readLine(DataCNFTag tag, DataCNFTag nextTag) {
	switch (tag.id >> 24) {
	case 0x7F:
		readSection(tag);
		break;
	case 0x7E:
		readEnc(tag);
		break;
	case 0x7D:
		readFace(tag, nextTag);
		break;
	case 0x00:
		break;
	default:
		readFile(tag, nextTag);
	}
}

void DataConfig::readLine(std::string line) {
	switch (line.at(0)) {
	case 0x2E:
		readFlag(&line[1]);
		break;
	case 0x3F:
		if (cnfHandler) cnfHandler->processAfp(&line[1], currentRegion, workdir);
		break;
	case 0x40:
		if (cnfHandler) cnfHandler->processQar(&line[1], currentRegion, workdir);
		break;
	default:
		if (cnfHandler) cnfHandler->processFile(line, currentRegion, workdir, NULL, 0);
	}
}

void DataConfig::initBinaryStream() {
	cnf = (DataCNF*)data;
	dataPtr += getCNFSize();
}

void DataConfig::initTextStream() {
	std::string cnfTxt = std::string((char*)data, dataSize);
	textStream.str(cnfTxt);
}

void DataConfig::readBinary() {
	initBinaryStream();

	for (int i = 0; i < cnf->numTags - 1; i++) {
		readLine(cnf->tags[i], cnf->tags[i + 1]);
	}
}

void DataConfig::readASCII() {
	initTextStream();

	std::string line;
	while (std::getline(textStream, line)) {
		readLine(line);
	}
}

void DataConfig::read(CNFTYPE type) {
	type == BINARY ? readBinary() : readASCII();
}