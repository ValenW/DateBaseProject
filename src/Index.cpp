#include "Index.h"

Index::Index(int totalSize, char *indexChar){

	size = totalSize;
	start = 0;
	dirty = true;

	for (int i = 0; i < size; i++)
		pageId[i] = i;

	if ((indexStream = fopen(indexChar, "wb+")) == NULL){
		printf("error, cann't open index file.\n");
	}

	globalDepth = 0;
	iotimes = 0;
	while (totalSize != 1) { totalSize = totalSize >> 1; globalDepth++; }
}

void Index::doubleIndex() {
#ifdef UPHASH
	if (size < MAXSIZE) {
		for (int i = (size << 1) - 1; i >= 0; i--)
			pageId[i] = pageId[i >> 1];
		dirty = true;
	}
	else {
		saveIfDirty();
		int temp[2];
		for (int i = size / MAXSIZE - 1; i >= 0; i--) {
			fseek(indexStream, i * sizeof(int) * MAXSIZE, SEEK_SET);
			fread(pageId, MAXSIZE * sizeof(int), 1, indexStream);
			iotimes++;
			for (int j = MAXSIZE - 1; j >= 0; j--) {
				temp[1] = temp[0] = pageId[j];
				fseek(indexStream, ((i * MAXSIZE + j) << 1) * sizeof(int), SEEK_SET);
				fwrite(&temp, sizeof(int), 2, indexStream);
				iotimes++;
			}
		}
		fseek(indexStream, start, SEEK_SET);
		fread(pageId, MAXSIZE * sizeof(int), 1, indexStream);
		iotimes++;
	}
#else
	if (size < MAXSIZE) {
		for (int i = 0; i < size; i++)
			pageId[i + size] = pageId[i];
		dirty = true;
	}
	else {
		saveIfDirty();

		int location = size / MAXSIZE;
		for (int i = 0; i < location; i++){
			fseek(indexStream, i * MAXSIZE * sizeof(int), SEEK_SET);
			fread(pageId, sizeof(int) * MAXSIZE, 1, indexStream);
			fseek(indexStream, (i * MAXSIZE + size) * sizeof(int), SEEK_SET);
			fwrite(pageId, MAXSIZE * sizeof(int), 1, indexStream);
		}
		start = (size << 1) - MAXSIZE;
	}
#endif
	size <<= 1;
	globalDepth++;
}

void Index::saveIfDirty(){
	if (!dirty) return;

	fseek(indexStream, start * sizeof(int), SEEK_SET);
	fwrite(pageId, MAXSIZE * sizeof(int), 1, indexStream);
	iotimes++;
	dirty = false;
}

int Index::getPageId(int hashKey){
	if (!(start <= hashKey && hashKey < start + MAXSIZE)) {

		saveIfDirty();
		int location = hashKey / MAXSIZE;

		fseek(indexStream, location * MAXSIZE * sizeof(int), SEEK_SET);
		fread(pageId, sizeof(int) * MAXSIZE, 1, indexStream);
		iotimes++;

		start = location * MAXSIZE;
	}
	return pageId[hashKey - start];
}

void Index::setPageId(int hashKey, int number){
	if (!(start <= hashKey && hashKey < start + MAXSIZE)) {

		saveIfDirty();
		int location = hashKey / MAXSIZE;

		fseek(indexStream, location * MAXSIZE * sizeof(int), SEEK_SET);
		fread(pageId, sizeof(int) * MAXSIZE, 1, indexStream);
		iotimes++;
		start = location * MAXSIZE;
	}
	pageId[hashKey - start] = number;
	dirty = true;
}

int Index::getGlobalDepth(){
	return globalDepth;
}

void Index::closeFile() {
	fclose(indexStream);
}

int Index::getsize() {
	return size;
}