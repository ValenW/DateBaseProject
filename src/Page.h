#ifndef PAGE_H_INCLUDED
#define PAGE_H_INCLUDED

#pragma once
#include "option.h"
#include "Func.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

typedef struct Rec_info {
	int16_t length;
	uint16_t offset;
} rec_info;

typedef struct Tong {
	char contents[CONCAP];
	uint8_t num;
	uint8_t deepth;
	uint16_t offset;
} tongStruct;

using namespace std;

class Page {

private:
	tongStruct tong;

	char tempStr[200];
	
	bool canAddRecord(char *rec);

	int getRecordOffset(int index);
	void setRecordOffset(int index, int number);

public:
	Page(int localDepth);

	char* getRecord(int index);

	bool addRecord(char *rec);

	int getRecordNumber();

	int getRecordLength(int index);

	void setRecordLength(int index, int number);

	int getLocalDepth();

	void setLocalDepth(int number);

	char* getContent();

	void adjustPage();

	void clearPage(int localDepth);
};

#endif // PAGE_H_INCLUDED
