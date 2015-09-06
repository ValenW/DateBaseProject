#pragma once
#include "Manager.h"

void swap(int *partKey, char(*str)[300], char *tempStr, int i, int j) {
	int tempPartKey = partKey[i];
	partKey[i] = partKey[j];
	partKey[j] = tempPartKey;
	strcpy(tempStr, str[i]);
	strcpy(str[i], str[j]);
	strcpy(str[j], tempStr);
}

void mySort(int *a, int l, int r, char(*str)[300], char *tempStr) {
	int i = l, j = r, mid = a[(r + l) >> 1];
	while (i <= j) {
		while (a[i] < mid) i++;
		while (a[j] > mid) j--;
		if (i <= j) swap(a, str, tempStr, i++, j--);
	}
	if (i < r) mySort(a, i, r, str, tempStr);
	if (l < j) mySort(a, l, j, str, tempStr);
}

Manager::Manager(char *inChar, char *outChar, char *testInChar, char *testOutChar, char* indexChar){
	if ((inStream = fopen(inChar, "r")) == NULL){
		printf("error,cann't open input file.\n");
	}
	if ((testInStream = fopen(testInChar, "r")) == NULL){
		printf("error,cann't open testinput.in\n");
	}
	if ((testOutStream = fopen(testOutChar, "w+")) == NULL){
		printf("error,cann't open testoutput.out\n");
	}

	start = clock();

	buffer = new Buffer(PAGENUM - 2, outChar);
	index = new Index(PAGENUM, indexChar);

	for (int i = 0; i < (PAGENUM - 2); i++) buffer->saveOutPage(i);

	//由于buffer的size为2^n - 1， 最后两页没有被存入硬盘，故在这里初始化
	buffer->setPageId(0, (PAGENUM - 2));
	buffer->saveOutPage(0);
	buffer->setPageId(0, (PAGENUM - 1));
	buffer->saveOutPage(0);
	buffer->setPageId(0, 0);

	pageNum = PAGENUM;
}

void Manager::insert(char* str) {
start: int globalDepth = index->getGlobalDepth();
	int key = getKey(str);
	int globalHashKey = getHash(key, globalDepth);
	int nowPageId = index->getPageId(globalHashKey);
	int location = buffer->ifPageExist(nowPageId);

	if (location == -1) {
		location = buffer->choseByClock();
		if (buffer->getDirty(location)) buffer->saveOutPage(location);
		buffer->readOutPage(location, nowPageId);
	}

	if (!(buffer->insertData(str, location))) {
		buffer->setPinCount(location);
		int localDepth = buffer->getLocalDepth(location);
		int localHash = getHash(key, localDepth);
		int total = 1 << globalDepth;
		int spiltLocation = buffer->choseByClock();

		if (buffer->getDirty(spiltLocation)) buffer->saveOutPage(spiltLocation);
		buffer->setPageId(spiltLocation, pageNum);

#ifdef UPHASH
		if (localDepth == globalDepth) {
			buffer->spiltPage(location, spiltLocation, globalHashKey << 1, localDepth);
			index->doubleIndex();
			index->setPageId(globalHashKey << 1 | 1, pageNum);
		}
		else {
			buffer->spiltPage(location, spiltLocation, localHash << 1, localDepth);

			int start = localHash << (globalDepth - localDepth);
			int size = 1 << (globalDepth - localDepth - 1);
			for (int j = 0; j < size; j++)
				index->setPageId(start + j, nowPageId);
			for (int j = size; j < (size << 1); j++)
				index->setPageId(start + j, pageNum);
	}
#else
		if (localDepth == globalDepth) {
			buffer->spiltPage(location, spiltLocation, globalHashKey, localDepth);
			index->doubleIndex();
			index->setPageId(globalHashKey + total, pageNum);
		}
		else {
			buffer->spiltPage(location, spiltLocation, localHash, localDepth);
			localHash += (1 << localDepth);
			int value = 0;
			int offset = value << (localDepth + 1);
			while (localHash + offset < total) {
				index->setPageId(localHash + offset, pageNum);
				value++;
				offset = value << (localDepth + 1);
			}
		}
#endif
		buffer->setDirty(location);
		buffer->setDirty(spiltLocation);
		pageNum++;
		goto start;
	}
}

void Manager::insertData() {
	char content[CAPACITY + 1];
	content[CAPACITY] = '\0';
	char str[300];
	int strCounter = 0;

	while (!feof(inStream)) {
		size_t current = fread(content, sizeof(char), CAPACITY, inStream);

		for (unsigned short i = 0; i < current; i++) {
			str[strCounter++] = content[i];
			if (content[i] == '\n') {
				str[strCounter - 1] = '\0';
				insert(str);
				hashNum++;
				strCounter = 0;
			}
		}
	}
}

void Manager::saveBuffer(){
	int size = buffer->getSize();
	for (int i = 0; i < size; i++)
		if (buffer->getDirty(i)) buffer->saveOutPage(i);
}

void Manager::saveIndex(){
	index->saveIfDirty();

	finish = clock();
	printf("\n*** Hash Finished ***\nCost %fs totally, that is %fmin.\n", (double)(finish - start) / CLOCKS_PER_SEC, (double)(finish - start) / CLOCKS_PER_SEC / 60.0);
	printf("Total hash num: %d.\nHash Speed: %f hash per second.\n", hashNum, (double)hashNum / ((double)(finish - start) / CLOCKS_PER_SEC));
	printf("Bucket Num: %d\n", pageNum);
	printf("Buffer I/O times: %d\n", buffer->iotimes);
	printf("Index Size: %d\n", index->getsize());
	printf("Index I/O times: %d\n", index->iotimes);
}

void Manager::query() {
	int temp = 0, t = 0, counter = 0;
	char str[30][300];
	int partKey[30];
	char tempStr[300];

	fscanf(testInStream, "%d", &t);
	queryNum = t;
	start = clock();

	while (t--) {
		fscanf(testInStream, "%d", &temp);
		counter = 0;
		int hashKey = getHash(temp, index->getGlobalDepth());
		int pageId = index->getPageId(hashKey);
		int location = buffer->ifPageExist(pageId);
		if (location == -1) {
			location = buffer->choseByClock();
			buffer->readOutPage(location, pageId);
		}

		Page* p = buffer->getPage(location);
		int size = p->getRecordNumber();

		for (int i = 0; i < size; i++) {
			if (getKey(p->getRecord(i)) == temp) {
				strcpy(str[counter], p->getRecord(i));
				partKey[counter] = getPartKey(str[counter]);
				counter++;
			}
		}

		mySort(partKey, 0, counter - 1, str, tempStr);

		for (int i = 0; i < counter; i++)
			fprintf(testOutStream, "%s\n", str[i]);

		fprintf(testOutStream, "-1\n");
	}

	buffer->closeFile();
	index->closeFile();
	fclose(inStream);
	fclose(testOutStream);
	fclose(testInStream);

	finish = clock();
	printf("\n*** Query Finished ***\nCost %fs totally, that is %fmin.\n", (double)(finish - start) / CLOCKS_PER_SEC, (double)(finish - start) / CLOCKS_PER_SEC / 60.0);
	printf("Query Speed: %f query per second.\n", (double)queryNum / ((double)(finish - start) / CLOCKS_PER_SEC));
	printf("Total Buffer I/O times: %d\n", buffer->iotimes);
	printf("Total Index I/O times: %d\n", index->iotimes);
}
