#pragma once
#include "Page.h"

Page::Page(int localDepth) {
	memset(tong.contents, '\0', sizeof(char) * CONCAP);

	tong.deepth = localDepth;
	tong.offset = 0;
	tong.num = 0;
}

bool Page::canAddRecord(char *rec) {
	return (tong.offset + sizeof(char) * strlen(rec) + sizeof(Rec_info) * (tong.num + 1) < CONCAP);
}

int Page::getRecordOffset(int index) {
	return ((rec_info*)(tong.contents + (CONCAP - sizeof(Rec_info) * (index + 1))))->offset;
}

void Page::setRecordOffset(int index, int number) {
	((rec_info*)(tong.contents + (CONCAP - sizeof(Rec_info) * (index + 1))))->offset = number;
}

char* Page::getRecord(int index) {
	int offset = getRecordOffset(index);
	int length = *(int16_t*)(tong.contents + CONCAP - sizeof(Rec_info) * (index + 1));

	memcpy(tempStr, tong.contents + offset, sizeof(char) * length);

	tempStr[length] = '\0';
	return tempStr;
}

bool Page::addRecord(char* rec) {
	if ((tong.offset + sizeof(char) * strlen(rec) + sizeof(Rec_info) * (tong.num + 1) < CONCAP)) {
		int temp = tong.num;
		int free_off = tong.offset;
		int rec_length = strlen(rec);

		memcpy(tong.contents + free_off, rec, sizeof(char) * rec_length);
		setRecordLength(temp, rec_length);
		setRecordOffset(temp, free_off);

		tong.offset += rec_length;
		tong.num++;
		return true;
	}
	return false;
}

int Page::getRecordNumber() {
	return tong.num;
}

int Page::getRecordLength(int index) {
	return *(int16_t*)(tong.contents + CONCAP - sizeof(Rec_info) * (index + 1));
}

void Page::setRecordLength(int index, int number) {
	((rec_info*)(tong.contents + CONCAP - sizeof(Rec_info) * (index + 1)))->length = number;
}

int Page::getLocalDepth() {
	return tong.deepth;
}

void Page::setLocalDepth(int number) {
	tong.deepth = number;
}

char *Page::getContent() {
	return (char *)&tong;
}

void Page::adjustPage() {
	int offset = 0;
	int rNum = tong.num, counter = rNum;
	int rLength, rOffset;
	int lastNotUse = 0;

	for (int i = 0; i < rNum; i++) {
		rLength = *(int16_t*)(tong.contents + CONCAP - sizeof(Rec_info) * (i + 1));

		if (rLength == -1) { counter--; continue; }

		bool flag = 0;
		for (int j = lastNotUse; j < i; j++)
			if (*(int16_t*)(tong.contents + CONCAP - sizeof(Rec_info) * (j + 1)) == -1) { lastNotUse = j; flag = 1; break; }

		if (flag == 1) {
			rOffset = getRecordOffset(i);

			memcpy(tong.contents + offset, tong.contents + rOffset, sizeof(char) * rLength);
			setRecordLength(i, -1);
			setRecordLength(lastNotUse, rLength);
			setRecordOffset(lastNotUse, offset);
		}
		offset += rLength;
	}

	tong.num = counter;
	tong.offset = offset;
}

void Page::clearPage(int localDepth) {
	memset(tong.contents, '\0', sizeof(char) * CONCAP);
	tong.deepth = localDepth;
	tong.offset = 0;
	tong.num = 0;
}
