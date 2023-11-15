#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define Max_Count 10
#define Max_Length 10

/*
Arrays that contain 8086 assembly instruction field;
*/
const char* Direc[] = { "SEGMENT", "ASSUME", "DW", "DB", "BOW", "ENDS", "END" };
const char* Opcd[] = { "MOV", "ADD", "SUB", "AND", "OR", "INC", "DEC", "CMP", "JMP", "JA" "JB", "JE", "JBE", "INT"};
const char* Reg16[] = { "AX", "BX", "CX", "DX", "SI", "DI", "CS", "DS" };
const char* Reg08[] = { "AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL" };

/*
Tables of Symbol, and Lable;
*/
char Symb[Max_Count][Max_Length];
char Lab[Max_Count][Max_Length];


/*
Counts of Symbol and Lable;
*/
int Symb_Count = 0;
int Lab_Count = 0;


/*
Function Prototypes;
*/
void pass1();
void pass2();

FILE* fp; // File pointer;

/*
Main Function;
*/
int main() {
	pass1();
	pass2();

	return 0;
}

/*
function : to read a source code file, then make a Symbol Table;
parameters : None;
output : None;
*/
void pass1() {
	char line[100];
	int index = 0, flag = 0;
	fp = fopen("prog2.txt", "r");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	while (fgets(line, 100, fp)) {
		flag = 0;
		char* token = strtok(line, "  :,\t\n");

		for (index = 0; Direc[index] != NULL; index++) {
			if (!strcmp(token, Direc[index])) {
				flag = 1;
				break;
			}
		}

		if (!flag) { // if first word != Direc;
			for (index = 0; Opcd[index] != NULL; index++) {
				if (!strcmp(token, Opcd[index])) {
					flag = 1;
					break;
				}
			}
		}

		if (!flag) { 
			char* token2 = strtok(NULL, " :,\t\n");
			if (!strcmp(token2, "DW") || !strcmp(token2, "DB")) { // if first word == Symbol;
				strcpy(Symb[Symb_Count], token);
				if (Symb_Count < Max_Count) Symb_Count++;
				flag++;
			}
			else { // if first word == Lable;
				strcpy(Lab[Lab_Count], token);
				if (Lab_Count < Max_Count) Lab_Count++;
				flag++;
			}
		}
	}

	fclose(fp);

	return;
}


/*
function : to read a source code file in words, then print what they are;
parameters : None;
output : None;
*/
void pass2() {
	char line[100];
	int index = 0, flag = 0;
	fp = fopen("prog2.txt", "r");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.");
		exit(1);
	}

	while (fgets(line, 100, fp)) {
		printf("%s", line);
		char* token = strtok(line, " :,\t\n");
		while (token != NULL) {
			flag = 0; // flag init;

			for (index = 0; Direc[index] != NULL; index++) { // if token == Directive;
				if (!strcmp(token, Direc[index])) {
					flag = 1;
					printf("Direc ");
					break;
				}
			}

			if (!flag) { // if token == Operation Code;
				for (index = 0; Opcd[index] != NULL; index++) {
					if (!strcmp(token, Opcd[index])) {
						flag = 1;
						printf("	Opcd ");
						break;
					}
				}
			}

			if (!flag) { // if token == 16bits Register;
				for (index = 0; Reg16[index] != NULL; index++) {
					if (!strcmp(token, Reg16[index])) {
						flag = 1;
						printf("Reg16 ");
						break;
					}
				}
			}

			if (!flag) { // if token == 8bits Register;
				for (index = 0; Reg08[index] != NULL; index++) {
					if (!strcmp(token, Reg08[index])) {
						flag = 1;
						printf("Reg08 ");
						break;
					}
				}
			}

			if (!flag) { // if token == Immediate;
				if(isdigit(token[0])) {
					flag = 1;
					printf("Num ");
				}
			}

			if (!flag && Symb_Count) { // if token == Symbol;
				for (index = 0; index <= Symb_Count; index++) {
					if (!strcmp(token, Symb[index])) {
						flag = 1;
						printf("Symb ");
						break;
					}
				}
			}

			if (!flag && Lab_Count) printf("Lable "); // if token == Lable;

			token = strtok(NULL, " :,\t\n");
		}
		printf("\n\n");
	}

	fclose(fp);

	return;
}