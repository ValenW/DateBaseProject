#include "Buffer.h"

Buffer::Buffer(int size, char* outChar) {
	this->size = size;
	pool = new Page*[size];
	pageId = new int[size];
	dirty = new bool[size];
	pin_count = new bool[size];
	ref_bit = new bool[size];
	currentPageId = 0;
	current = 0;
	iotimes = 0;

	if ((outStream = fopen(outChar, "wb+")) == NULL){
		printf("error, cann't open output file.\n");
	}

	int localDepth = 3;
	if (size == 126) localDepth = 7;

	memset(pin_count, 0, sizeof(bool) * size);
	memset(ref_bit, 0, sizeof(bool) * size);
	memset(dirty, 0, sizeof(bool) * size);
	for (int i = 0; i < size; i++) {
		pool[i] = new Page(localDepth);
		pageId[i] = i;
	}
};

int Buffer::getSize() {
	return size;
};

bool Buffer::insertData(char* str, int f_offset) {
	ref_bit[f_offset] = true;
	dirty[f_offset] = true;
	return pool[f_offset]->addRecord(str);
};

int Buffer::ifPageExist(int pageNum) {
	for (int i = 0; i < size; i++)
		if (pageId[i] == pageNum) return i;
	return -1;
};

bool Buffer::getDirty(int frameNum) {
	return dirty[frameNum];
};

int Buffer::choseByClock() {
	while (true) {
		if (!(ref_bit[current] || pin_count[current])) break;
		else if (!pin_count[current]) ref_bit[current] = false;
		current = (current + 1) % size;
	}

	int temp = current;

	current = (current + 1) % size;

	return temp;
};

void Buffer::setPinCount(int frameNum){
	pin_count[frameNum] = true;
}

int Buffer::getPageId(int offset){
	return pageId[offset];
}

void Buffer::setPageId(int f_offset, int pId){
	pageId[f_offset] = pId;
}

void Buffer::setDirty(int frameNum){
	dirty[frameNum] = true;
}

void Buffer::clearPinCount(int frameNum){
	pin_count[frameNum] = false;
}

void Buffer::saveOutPage(int location){
	int offset = (pageId[location] - currentPageId) * CAPACITY;
	fseek(outStream, offset, SEEK_CUR);
	fwrite(pool[location]->getContent(), CAPACITY, 1, outStream);

	ref_bit[location] = false;
	currentPageId = pageId[location] + 1;
	dirty[location] = false;

	iotimes++;
}

void Buffer::readOutPage(int location, int pageId){
	int offset = (pageId - currentPageId) * CAPACITY;

	fseek(outStream, offset, SEEK_CUR);
	fread(pool[location]->getContent(), sizeof(char) * CAPACITY, 1, outStream);

	this->pageId[location] = pageId;
	this->dirty[location] = false;
	currentPageId = pageId + 1;

	iotimes++;
}

void Buffer::clearPage(int frameNum, int localDepth){
	pool[frameNum]->clearPage(localDepth);
}

void Buffer::spiltPage(int oldLocation, int newLocation, int hashKey, int localDepth) {
	int slotSize = pool[oldLocation]->getRecordNumber();
	char* str;

	pool[newLocation]->clearPage(localDepth + 1);
	pool[oldLocation]->setLocalDepth(localDepth + 1);

	for (int i = 0; i < slotSize; i++){
		str = pool[oldLocation]->getRecord(i);

		if (!(getHash(getKey(str), localDepth + 1) == hashKey)) {
			insertData(str, newLocation);
			pool[oldLocation]->setRecordLength(i, -1);
		}
	}
	pool[oldLocation]->adjustPage();

	pin_count[newLocation] = false;
	pin_count[oldLocation] = false;
}

Page* Buffer::getPage(int f_offset){
	return pool[f_offset];
}

int Buffer::getLocalDepth(int location){
	return pool[location]->getLocalDepth();
}

void Buffer::setLocalDepth(int location, int localDepth){
	pool[location]->setLocalDepth(localDepth);
	dirty[location] = true;
}

void Buffer::closeFile() {
	fclose(outStream);
}
