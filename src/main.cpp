#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "option.h"
#include "Page.h"
#include "Manager.h"

char inStreamChar[100] = "lineitem.tbl";
char outStreamChar[100] = "result.fil";
char testInStreamChar[100] = "testinput.in";
char testOutStreamChar[100] = "testoutput.out";
char indexStreamChar[100] = "hashindex.out";

#define _CRT_SECURE_NO_WARNINGS

int main(int argc, char *argv[]) {
	char path[200];
	if (argc != 2) {
		printf("No path input, use default path.\n");
		strcpy_s(path, DEFAULTPATH);
	}
	else if (argv[1][strlen(argv[1]) - 1] != '\\') {
		strcpy_s(path, argv[1]);
		path[strlen(argv[1])] = '\\';
		path[strlen(argv[1]) + 1] = '\0';
	}
	else {
		strcpy_s(path, argv[1]);
	}

	char temp[200];
	strcpy_s(temp, path);
	strcat_s(temp, inStreamChar);
	strcpy_s(inStreamChar, temp);

	strcpy_s(temp, path);
	strcat_s(temp, outStreamChar);
	strcpy_s(outStreamChar, temp);

	strcpy_s(temp, path);
	strcat_s(temp, testInStreamChar);
	strcpy_s(testInStreamChar, temp);

	strcpy_s(temp, path);
	strcat_s(temp, testOutStreamChar);
	strcpy_s(testOutStreamChar, temp);

	strcpy_s(temp, path);
	strcat_s(temp, indexStreamChar);
	strcpy_s(indexStreamChar, temp);

	clock_t start, finish;

#ifdef UPHASH
	char s[20] = "Hash From Up";
#else
	char s[20] = "Hash From Low";
#endif
	printf("\n*** The Programe Start ***\nPageNum: %d.\nHash Way: %s\n", PAGENUM, s);

	start = clock();
	Manager m(inStreamChar, outStreamChar, testInStreamChar, testOutStreamChar, indexStreamChar);
	m.insertData();
	m.saveBuffer();
	m.saveIndex();
	m.query();

	finish = clock();
	double cost = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("\n***** The Programe Finished ***\nCost %fs totally, that is %fmin.\n", cost, cost / 60.0);
	return 0;
}
