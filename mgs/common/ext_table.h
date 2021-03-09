#pragma once
#include <inttypes.h>

struct EXT_TABLE {
	const char* name;
	int id;
};

const EXT_TABLE ext_table[15] = {
	{"bin",  0x01 },
	{"cv2",  0x02 },
	{"evm",  0x04 },
	{"far",  0x05 },
	{"gcx",  0x06 },
	{"hzx",  0x07 },
	{"kms",  0x0A },
	{"lt2",  0x0B },
	{"mar",  0x0C },
	{"o2d",  0x0E },
	{"row",  0x11 },
	{"sar",  0x12 },
	{"tri",  0x13 },
	{"var",  0x15 },
	{"zms",  0x19 },
};

inline
const char* getExtForID(uint8_t id) {
	for (int i = 0; i < 15; i++) {
		if (ext_table[i].id == id)
			return ext_table[i].name;
	}
	return "";
}

inline
uint8_t getIDforExt(const char* ext) {
	for (int i = 0; i < 15; i++) {
		if (!strcmp(ext_table[i].name, ext))
			return ext_table[i].id;
	}
	return -1;
}