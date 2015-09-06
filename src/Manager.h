#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#include "option.h"
#include "Buffer.h"
#include "Index.h"
#include "Page.h"
#include "Func.h"
#include <time.h>
#include <stdio.h>
#include <fstream>

class Manager {
private:
	FILE *inStream;

	FILE *testInStream;
	FILE *testOutStream;
	Buffer *buffer;
	Index *index;
	long start, finish;
	unsigned long pageNum;
	unsigned long hashNum;
	unsigned long queryNum;
public:
	Manager(char *inChar, char *outChar, char *testInChar, char *testOutChar, char* indexChar);
	void insertData();
	void saveBuffer();
	void insert(char*str);
	void saveIndex();
	void query();
};

#endif // MANAGER_H_INCLUDED
