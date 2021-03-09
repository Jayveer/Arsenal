#include "stage.h"

Stage::Stage(std::string filename) : Face(filename) {}

Stage::Stage(std::string filename, uint32_t sector) : Face(filename, sector) {}

Stage::~Stage() {}

void Stage::open() {
	std::ifstream stageDat;
	stageDat.open(filename, std::ios::binary);

	int size = sizeof(FaceHeader);
	stageDat.read((char*)&this->header, size);
	uint32_t keyA = header.timestamp;
	uint32_t keyB = keyA ^ 0xF0F0;
	keyA = decryptor.decodeBuffer(keyA, keyB, 0, 0x0C, (uint8_t*)&header.version);
		
	table.resize(header.numPages);
	size = sizeof(FaceTable) * header.numPages;
	stageDat.read((char*)&table[0], size);
	keyA = decryptor.decodeBuffer(keyA, keyB, 0, size, (uint8_t*)&table[0]);
}
/*
void Stage::extract(uint16_t pageID, std::string output) {
	if (pageID > header.numPages) return;

	updateDir("stage", output);
	updateDir(table[pageID].name, output);

	int64_t nextOffset = getNextPageOffset(pageID);
	int64_t offset = table[pageID].offset * sector;

	int64_t size = nextOffset - offset;
	extractFiles(size, offset, output, true);
}*/

void Stage::extractAll(std::string output) {
	for (int i = 0; i < header.numPages; i++) {
		extract(i, output, "stage", true);
	}
}