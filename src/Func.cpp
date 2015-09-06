#include "Func.h"

int getPartKey(char* str) {
	char *begin = strchr(str, '|') + 1;
	int result = 0;

	for (int i = 0; begin[i] != '|'; i++) {
		result *= 10;
		result += begin[i] - '0';
	}

	return result;
}

int getHash(int key, int depth) {
#ifdef UPHASH
	__int32 num = key & 0x007fffff;
	num >>= (23 - depth);
	return num;
#else
	return key % (1 << depth);
#endif
}

int getKey(char*str){
	int result = 0;
	int counter = 0;

	while (str[counter] != '|') {
		result *= 10;
		if (str[counter] <= '9' && str[counter] >= '0') result += str[counter] - '0';
		counter++;
	}

	return result;
}
