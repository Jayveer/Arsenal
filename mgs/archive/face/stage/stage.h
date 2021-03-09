#pragma once
#include "../face.h"

class Stage : public Face {
public:
	Stage(std::string filename);
	Stage(std::string filename, uint32_t sector);
	~Stage();

	void open();
	void extractAll(std::string output = "");
	//void extract(uint16_t pageID, std::string output = "");	
};