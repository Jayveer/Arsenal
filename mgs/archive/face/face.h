#pragma once
#include <time.h>

#include "../archive.h"
#include "../../config/cnf.h"
#include "../../common/game/game.h"
#include "../../common/decryptor/decryptor.h"
#include "../../3rdparty/zlib/wrapper/ZlibWrapper.h"

struct FaceHeader {
	uint32_t timestamp; //interchangable with strcode
	uint16_t version;
	uint16_t pageSize;  //multiplied by sector size
	uint16_t numPages;
	uint16_t unknownA;
	uint32_t unknownB;
};

#pragma pack(push, 4)
struct FaceTable {
	char name[8];
	int32_t offset;
};
#pragma pop

class Face : public CNFHandler, public Archive {
protected:
	FaceHeader header;
	Decryptor decryptor;
	std::string filename;
	uint32_t sector = 0x800;
	std::vector<FaceTable> table;

	int64_t getNextPageOffset(uint16_t pageID);
	void decryptPage(uint8_t* src, int64_t size);
	void decryptPageCNF(const std::string& stageName, uint8_t* src);
	void extractFiles(int64_t size, int64_t offset, std::string& output, bool isEncrypted = 0);

	int32_t getSectorSize();
	int64_t getNextSectorOffset(int64_t currentOffset);
	void processQar(std::string filename, std::string region, std::string* workDir);
	void processAfp(std::string filename, std::string region, std::string* workDir);
	void processVram(std::string filename, std::string region, std::string* workDir);
	void processEnc(std::string region, uint8_t** section, int64_t size, int64_t sectionSize);
	void processFile(std::string filename, std::string region, std::string* workDir, uint8_t* fileData, int size);
	void processFace(std::string filename, int16_t page, int64_t offset, std::string region, std::string* workDir);
private:
	int64_t getMaxPageSize(int64_t offset);
	void initPageInfo(std::ifstream& faceDat);
	int64_t getMaxPageSize(std::ifstream& faceDat);
	uint8_t* decompressPage(uint8_t* compressedPage, int64_t compressedSize, int64_t decompressedSize);
public:
	~Face();
	Face(std::string filename);
	Face(std::string filename, uint32_t sector);

	std::string getFilename();
	std::string getCreatedDate();

	void open();
	void extractAll(std::string output = "");
	void extract(uint16_t pageID, std::string output, std::string dirName, bool isEncrypted);

};