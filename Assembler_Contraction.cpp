#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
Arrays that contain 8086 assembly instruction field;
*/
const char* PCMD[] = { "SEGMENT", "ASSUME", "DW", "DB", "BOW", "ENDS", "END" };
const char* CMD[] = { "MOV", "ADD", "JMP", "CMP", "JBE", "INC", };
const char* R16[] = { "AX", "BX", "CX", "DX", "SI", "DI", "BP", "SP", "CS", "DS"};
const char* R8[] = { "AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL" };

FILE* fp;

void pass1(FILE* filename);
void pass2(char* str);

int main() {

	fp = fopen("prog1.txt", "r");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	pass1(fp);

	return 0;
}

/*
function : to token a file into strings and print them;
parameters : a file pointer;
output : None;
*/
void pass1(FILE* filename) {
	char line[100];
	while (fgets(line, 100, filename)) {
		printf("\n%s", line);
		char* token = strtok(line, "  :,\t\n");
		while (token != NULL) {
			pass2(token);
			token = strtok(NULL, " :,\t\n");
		}
	}

	return;
}

/*
function : to ditermine what intput is;
parameters : a string pointer;
output : None;
*/
void pass2(char* str) {
	int i;

	if (str[0] >= 48 && str[0] <= 57) {
		printf("NUM ");
		return;
	}

	for (i = 0; CMD[i] != NULL; i++) {
		if (!strcmp(str, CMD[i])) {
			printf("	CMD	");
			return;
		}
	}

	for (i = 0; PCMD[i] != NULL; i++) {
		if (!strcmp(str, PCMD[i])) {
			printf("PCMD ");
			return;
		}
	}

	for (i = 0; i<10; i++) {
		if (!strcmp(str, R16[i])) {
			printf("R16 ");
			return;
		}
	}

	for (i = 0; i<8; i++) {
		if (!strcmp(str, R8[i])) {
			printf("R8 ");
			return;
		}
	}
	printf("SYMB ");

	return;
}