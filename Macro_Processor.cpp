#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Arrays that contain macros
*/
const char* MACRO[] = { "AM", "BM" };

FILE* in_fp;
FILE* out_fp[3];

void pass1();
void pass2();

int main() {

	pass1();
	pass2();

	return 0;
}

/*
function : to read a source_code file, then make MDT;
parameters : None;
output : None;
*/
void pass1() {
	char line[100];
	in_fp = fopen("src_code.txt", "r");
	if (in_fp == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	while (fgets(line, sizeof(line), in_fp)) {
		if (strstr(line, MACRO[0]) && strstr(line, "MACRO")) {
			fgets(line, sizeof(line), in_fp);
			out_fp[0] = fopen("AM.txt", "w");
			if (out_fp[0] == NULL) {
				printf("파일을 열 수 없습니다.");
				exit(1);
			}
			while (!strstr(line, "ENDM")) {
				fputs(line, out_fp[0]);
				fgets(line, sizeof(line), in_fp);
			}
			fclose(out_fp[0]);
		}

		if (strstr(line, MACRO[1]) && strstr(line, "MACRO")) {
			fgets(line, sizeof(line), in_fp);
			out_fp[1] = fopen("BM.txt", "w");
			if (out_fp[1] == NULL) {
				printf("파일을 열 수 없습니다.");
				exit(1);
			}
			while (!strstr(line, "ENDM")) {
				fputs(line, out_fp[1]);
				fgets(line, sizeof(line), in_fp);
			}
			fclose(out_fp[1]);
		}

	}

	fclose(in_fp);

	return;
}

/*
function : to read a source_code file, then make object_code;
parameters : None;
output : None;
*/
void pass2() {
	char line[100];

	in_fp = fopen("src_code.txt", "r");
	if (in_fp == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	out_fp[2] = fopen("obj_code.txt", "w");
	if (out_fp[2] == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	while (fgets(line, sizeof(line), in_fp)) {
		if (!strstr(line, "MACRO")) {
			if (strstr(line, MACRO[0])) {
				out_fp[0] = fopen("AM.txt", "r");
				if (out_fp[0] == NULL) {
					printf("파일을 열 수 없습니다.");
					exit(1);
				}
				while (fgets(line, sizeof(line), out_fp[0])) fputs(line, out_fp[2]);
				fclose(out_fp[0]);
			}

			else if (strstr(line, MACRO[1])) {
				out_fp[1] = fopen("BM.txt", "r");
				if (out_fp[1] == NULL) {
					printf("파일을 열 수 없습니다.");
					exit(1);
				}
				while (fgets(line, sizeof(line), out_fp[1])) fputs(line, out_fp[2]);
				fclose(out_fp[1]);
			}

			else fputs(line, out_fp[2]);
		}
		else {
			while (!strstr(line, "ENDM")) fgets(line, sizeof(line), in_fp);
		}
	}

	fclose(in_fp);
	fclose(out_fp[2]);

	return;
}