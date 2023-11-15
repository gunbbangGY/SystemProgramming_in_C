#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>

int main() {
	FILE* fp;
    char line[1000];
	int Cnt_word = 0, Cnt_char = 0, Cnt_num = 0, i = 0, flag = 0;

    fp = fopen("five_line.txt", "r");
    if (fp == NULL) {
        printf("파일을 열 수 없습니다.");
        exit(1);
    }
 
    while (fgets(line, sizeof(line), fp) != NULL) {
        for (i = 0; line[i] != '\0'; i++) {
            Cnt_char++;
            if (line[i] == ' ' || line[i] == '\n') {
                if (flag) {
                    flag = 0;
                    Cnt_word++;
                }
            }
            else flag = 1;

            if (line[i] >= '0' && line[i] <= '9') Cnt_num++;
        }
    }

    fclose(fp);

    printf("단어 개수: %d\n", Cnt_word);
    printf("문자 개수: %d\n", Cnt_char);
    printf("숫자 개수: %d\n", Cnt_num);

    return 0;

}