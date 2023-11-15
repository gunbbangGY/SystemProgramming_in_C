#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void strtmp(char* str1, char* str2);
int main() {
	FILE* fp;
	char line[100], array[50][10], input[15];
	int count1, count2, num = 0;

	fp = fopen("five_line.txt", "r");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	while (fgets(line, 100, fp)) {
		char* token = strtok(line, " \n");
		while (token != NULL) {
			strcpy(array[num], token);
			num++;
			token = strtok(NULL, " \n");
		}
	}

	for (count1 = 0; count1 < num; count1++) {
		for (count2 = count1 + 1; count2 < num-1; count2++) {
			if (strcmp(array[count1], array[count2]) > 0) {
				strtmp(array[count1], array[count2]);
			}
		}
	}

	for (count1 = 0; count1 < num; count1++) printf("%s\n", array[count1]);

	printf("\n\n찾고 싶은 단어 : ");
	scanf("%s", input);


	for (count1 = 0; count1 < num; count1++) {
		if (strcmp(input, array[count1]) == 0) {
			printf("%s는 %d째 단어", input, count1 + 1);
			break;
		}
	}
	printf("%s는 리스트에 없는 단어", input);

	return 0;
}

/*
function : to replace the values of the two string pointers
parameters : two different string pointers
output : None
*/
void strtmp(char* str1, char* str2) {
	char temp[100];
	if (strlen(str1) < 100 && strlen(str2) < 100) {
		strcpy(temp, str1);
		strcpy(str1, str2);
		strcpy(str2, temp);
	}

	return;
}