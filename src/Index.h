#ifndef INDEX_H_INCLUDED
#define INDEX_H_INCLUDED

#pragma once
#include "option.h"
#include <string.h>
#include <stdio.h>

class Index {

private:
	int size;
	int globalDepth;
	FILE *indexStream;
	int pageId[MAXSIZE];
	int start;
	bool dirty;

public:
	unsigned long iotimes;

	Index(int totalSize, char *indexChar);

	void doubleIndex();

	void saveIfDirty();

	int getPageId(int hashKey);

	void setPageId(int hashKey, int number);

	int getGlobalDepth();

	void closeFile();

	int getsize();
};

#endif



