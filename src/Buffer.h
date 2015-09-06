#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include "option.h"
#include "Page.h"

class Buffer {
private:
	Page** pool;
	int* pageId;

	bool* ref_bit;
	bool* pin_count;
	bool* dirty;

	int size;

	FILE *outStream;
	int currentPageId;

	int current;

public:
	int iotimes;

	Buffer(int size, char*outChar);
	int getSize();

	bool insertData(char* str, int f_offset);
	int ifPageExist(int pageNum);

	int choseByClock();

	Page* getPage(int frameNum);

	bool getDirty(int frameNum);
	void setDirty(int frameNum);

	void setPinCount(int frameNum);
	void clearPinCount(int frameNum);

	int getPageId(int frameNum);
	void setPageId(int frameNum, int pId);

	int getLocalDepth(int frameNum);
	void setLocalDepth(int location, int localDepth);

	void adjustPage(int frameNum);
	void clearPage(int frameNum, int localDepth);

	void saveOutPage(int frameNum);
	void readOutPage(int frameNum, int pageId);

	void spiltPage(int oldLocation, int newLocation, int hashKey, int localDepth);

	void closeFile();
};

#endif // BUFFER_H_INCLUDED
