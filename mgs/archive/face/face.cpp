#include "face.h"

Face::Face(std::string filename) {
	this->filename = filename;
}

Face::Face(std::string filename, uint32_t sector) {
	this->filename = filename;
	this->sector = sector;
}

Face::~Face() {}

std::string Face::getFilename() {
	return this->filename;
}

std::string Face::getCreatedDate() {
	std::string createdDate;
	createdDate.resize(20);
	time_t rawtime = (time_t)this->header.timestamp;
	tm ptm;
	gmtime_s(&ptm, &rawtime);

	sprintf_s(&createdDate[0],
		20,
		"%02d/%02d/%04d %02d:%02d:%02d",
		ptm.tm_mday,
		ptm.tm_mon,
		(1900 + ptm.tm_year),
		ptm.tm_hour,
		ptm.tm_min,
		ptm.tm_sec);

	return createdDate;
}

void Face::open() {
	std::ifstream faceDat;
	faceDat.open(filename, std::ios::binary);
	faceDat.read((char*)&this->header, sizeof(FaceHeader));

	table.resize(header.numPages);
	int size = sizeof(FaceTable) * header.numPages;
	faceDat.read((char*)&this->table[0], size);
	faceDat.close();
}

uint8_t* Face::decompressPage(uint8_t* compressedPage, int64_t compressedSize, int64_t decompressedSize) {
	int size;
	ZlibWrapper zlib;
	uint8_t* decompressedPage = new uint8_t[decompressedSize];

	zlib.zlibInflate((char*)(compressedPage), (char*)decompressedPage, compressedSize, decompressedSize, &size, 1);

	return decompressedPage;
}

void Face::decryptPage(uint8_t* src, int64_t size) {
	uint16_t key = *(uint16_t*)src;

	decryptor.decodePage(key, 0, size, src);
	src[0] = 0x78; src[1] = 0x9C;
}

void Face::decryptPageCNF(const std::string& stageName, uint8_t* src) {
	uint32_t keyA = strcode(stageName.c_str());
	uint32_t keyB = header.timestamp;

	int32_t numTags = *(uint32_t*)src;
	decryptor.decodeCNF(keyA, keyB, 0, 4, (uint8_t*)&numTags);

	int64_t cnfHeaderSize = sizeof(DataCNF) + sizeof(DataCNFTag) * numTags;
	decryptor.decodeCNF(keyA, keyB, 0, cnfHeaderSize, src);
}

void Face::extractFiles(int64_t size, int64_t offset, std::string& output, bool isEncrypted) {
	std::ifstream fileDat;
	fileDat.open(filename, std::ios::binary);
	fileDat.seekg(offset);

	uint8_t* pageData = new uint8_t[size];
	fileDat.read((char*)pageData, size);
	fileDat.close();

	if (isEncrypted) decryptPageCNF(getCurrentDir(output), pageData);

	DataConfig cnf = DataConfig(pageData, size);
	cnf.setHandler(this);
	cnf.setWorkDir(output);
	cnf.read(BINARY);

	delete[] pageData;
}

void Face::extract(uint16_t pageID, std::string output, std::string dirName, bool isEncrypted) {
	if (pageID > header.numPages) return;

	updateDir(dirName, output);
	updateDir(table[pageID].name, output);

	int64_t nextOffset = getNextPageOffset(pageID);
	int64_t offset = table[pageID].offset * sector;

	int64_t size = nextOffset - offset;
	extractFiles(size, offset, output, isEncrypted);
}

void Face::extractAll(std::string output) {
	for (int i = 0; i < header.numPages; i++) {
		extract(i, output, "face", false);
	}
}

int64_t Face::getNextPageOffset(uint16_t pageID) {
	return (pageID == header.numPages - 1) ? getFileSize(filename) : table[pageID + 1].offset * sector;
}

/* CNF */
int64_t Face::getNextSectorOffset(int64_t currentOffset) {
	return getAlignment(currentOffset, sector);
}

int32_t Face::getSectorSize() {
	return this->sector;
}

void Face::processQar(std::string filename, std::string region, std::string* workDir) {}

void Face::processFace(std::string filename, int16_t page, int64_t offset, std::string region, std::string* workDir) {}

void Face::processVram(std::string filename, std::string region, std::string* workDir) {}

void Face::processAfp(std::string filename, std::string region, std::string* workDir) {	}

void Face::processFile(std::string filename, std::string region, std::string* workDir, uint8_t* fileData, int size) {
	writeDataToFile(fileData, size, filename, *workDir);
}

void Face::processEnc(std::string region, uint8_t** section, int64_t size, int64_t sectionSize) {
	decryptPage(*section, size);
	*section = decompressPage(*section, size, sectionSize);
}