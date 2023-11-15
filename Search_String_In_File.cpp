#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main() {
	FILE* fp;
	char line[100];
	char input[15];

	int col = 0, num = 0;

	scanf("%s", input);

	fp = fopen("data.txt", "r");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	while (fgets(line, 100, fp)) {
		col++;
		num = 0;
		char* token = strtok(line, " \n");

		while (token != NULL) {
			num++;
			if (strcmp(input, token) == 0) {
				printf("(%d, %d)", col, num);
			}
			token = strtok(NULL, " \n");
		}
	}

	fclose(fp);

	return 0;
}