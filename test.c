#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// [] 처리 불가

struct Register {
    char Register_name[3];
    char Word_type[2];
    char Register_number[4];
} reg[20];

struct Instruction {
    char Opcd[6];
    char Dest[2];
    char Sour[2];
    char Word_type[2];
    char Binary_code[3];
    char Length[2];
    char Mod_reg[9];
} inst[100];

struct Symbol {
    char Symbol_name[10];
    char Word_type[2];
    int Location;
    char Data[10];
} symb[20];

struct Sentence {
    char Label[10];
    char _Operator[10];
    char Operand[3][10];
} sen;

const char* Directive[4] = { "ASSUME", "SEGMENT", "ENDS", "END" };
const char* R[15] = { "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH", "AX", "CX", "DX", "BX", "BX", "SI", "DI" };

int Register_count = 0;
int Instruction_count = 0;
int Symbol_count = 0;
int LC = 0;

FILE* fp_code, * fp_reg, * fp_inst;

int btoi(char* dig) {
    register int i = 0, ret = 0;
    while (*(dig + i) != '\0') {
        if (*(dig + i) == '1') ret += pow((double)2, (double)strlen(dig + i) - 1);
        i++;
    }
    return ret;
}

/*
function : to reset a global variable 'sen';
input : none;
output : none;
*/
void Clear_struct() {
    strcpy(inst[0].Opcd, "");
    strcpy(inst[0].Dest, "");
    strcpy(inst[0].Sour, "");
    strcpy(inst[0].Word_type, "");
    strcpy(inst[0].Binary_code, "");
    strcpy(inst[0].Mod_reg, "");
    strcpy(inst[0].Length, "");

    strcpy(sen.Label, "");
    strcpy(sen._Operator, "");
    for (int i = 0; i < 3; i++) strcpy(sen.Operand[i], "");
}

/*
function : to read files then make reg_tbl, inst_tbl;
input : none;
output : none;
*/
void init() {
    int count = 0;

    fp_reg = fopen("reg_tbl.txt", "r");
    fp_inst = fopen("inst_tbl.txt", "r");
    if (fp_reg == NULL || fp_inst == NULL) {
        printf("can't open a file");
        exit(1);
    }

    while (!feof(fp_reg)) {
        fscanf(fp_reg, "%s%s%s\n", reg[count].Register_name, 
            reg[count].Word_type, reg[count].Register_number);
        count++;
    }
    Register_count = count;

    count = 1;

    while (!feof(fp_inst)) {
        fscanf(fp_inst, "%6s%2s%2s%2s%2s%3s%9s\n", inst[count].Opcd,
            inst[count].Word_type, inst[count].Dest, inst[count].Sour,
            inst[count].Length, inst[count].Binary_code, inst[count].Mod_reg);
        count++;
    }
    Instruction_count = count;

    fclose(fp_reg);
    fclose(fp_inst);

    return;
}

/*
function : to analyze an operand;
parameters : an operand;
output : specific number;
*/
int anal(char* operand) {
    int i = 0;
    // operand == immediate;
    if (isdigit(operand[0])) return 0;
    // operand == CS;
    if (strcmp(operand, "CS") == 0) return 5;
    // operand == DS;
    if (strcmp(operand, "DS") == 0) return 6;

    for (i = 0; i < Register_count; i++) {
        if (!strncmp(operand, reg[i].Register_name, 5)) {
            if (!strncmp(reg[i].Register_name, "AX", 2)) {
                inst[0].Word_type[0] = 'w';
                return 4;
            }
            if (!strncmp(reg[i].Register_name, "AL", 2)) {
                inst[0].Word_type[0] = 'b';
                return 4;
            }
            // operand == 16bits register;
            if (!strncmp(reg[i].Word_type, "w", 1)) return 1;
            // opernad == 8bits register;
            if (!strncmp(reg[i].Word_type, "b", 1)) return 2;
        }
    }
    // operand == Memory;
    if (operand[0] != '\0') return 3;
    // operand == None;
    else return 7;
}

/*
function : to analyze an operand;
parameters : an operand;
output : specific number;
*/
int Check_field(char* line) {
    int field_count = 0, Symb_flag = 0, i, j, k;
    char* token, * field[5] = { NULL, NULL, NULL, NULL, NULL };

    Clear_struct();
    // if Directive in line
    for (i = 0; i < 4; i++) {
        if (strstr(line, Directive[i])) return -1;
    }

    if (strstr(line, "DW")) {
        token = strtok(line, " ,:\t\n");
        strcpy(symb[Symbol_count].Symbol_name, token);
        strcpy(symb[Symbol_count].Word_type, "w");
        token = strtok(NULL, " ,:\t\n");
        token = strtok(NULL, " ,:\t\n");
        strcpy(symb[Symbol_count].Data, token);
        return 0;
    }

    if (strstr(line, "DB")) {
        token = strtok(line, " \t\n");
        strcpy(symb[Symbol_count].Symbol_name, token);
        strcpy(symb[Symbol_count].Word_type, "b");
        token = strtok(NULL, " ,:\t\n");
        token = strtok(NULL, " ,:\t\n");
        strcpy(symb[Symbol_count].Data, token);
        return 0;
    }

    if (strstr(line, "INT")) {
        token = strtok(line, " ,:\t\n");
        token = strtok(NULL, " ,:\t\n");
        strncpy(sen.Operand[0], token, 3);
        return 30;
    }
    if (strstr(line, "JMP")) {
        token = strtok(line, " ,:\t\n");
        token = strtok(NULL, " ,:\t\n");
        strcpy(sen.Operand[0], token);
        return 26;
    }
    if (strstr(line, "JA")) {
        token = strtok(line, " \t\n");
        token = strtok(NULL, " \t\n");
        strcpy(sen.Operand[0], token);
        return 27;
    }
    if (strstr(line, "JE")) {
        token = strtok(line, " \t\n");
        token = strtok(NULL, " \t\n");
        strcpy(sen.Operand[0], token);
        return 29;
    }
    if (strstr(line, "JB")) {
        token = strtok(line, " \t\n");
        token = strtok(NULL, " \t\n");
        strcpy(sen.Operand[0], token);
        return 28;
    }

    if (line[0] != '\t') {
        token = strtok(line, " ,:\t\n");
        strcpy(sen.Label, token);
        token = strtok(NULL, " ,:\t\n");
    }
    else {
        token = strtok(line, " ,:\t\n");
        strcpy(sen.Label, "\0");
    }

    while (token != NULL && field_count < 5) {
        field[field_count++] = token;
        token = strtok(NULL, " ,:\t\n");
    }

    for (i = 1; i < Instruction_count ; i++) {
        if (!strncmp(field[0], inst[i].Opcd, 3)) {
            strcpy(sen._Operator, field[0]);
            strcpy(inst[0].Opcd, field[0]);
            break;
        }
    }

    j = 1;
    k = 0;

    while (j < 5 && k < 3) {
        if (field[j] == NULL || field[j][0] == '\0') strcpy(sen.Operand[k], "\0");
        else strcpy(sen.Operand[k], field[j]);
        j++;
        k++;
    }

    switch (anal(sen.Operand[0])) {
    case 1:
        strcpy(inst[0].Dest, "r");
        strcpy(inst[0].Word_type, "w");
        break;
    case 2:
        strcpy(inst[0].Dest, "r");
        strcpy(inst[0].Word_type, "b");
        break;
    case 3:
        strcpy(inst[0].Dest, "m");
        break;
    case 4:
        strcpy(inst[0].Dest, "a");
        break;
    case 5:
        strcpy(inst[0].Dest, "C");
        strcpy(inst[0].Word_type, "w");
        return 54;
    case 6:
        strcpy(inst[0].Dest, "D");
        strcpy(inst[0].Word_type, "w");
        return 55;
    case 7:
        strcpy(inst[0].Dest, "N");
        strcpy(inst[0].Word_type, "N");
        break;
    default:
        printf("Source_code Error\n");
        return -1;
    }

    switch (anal(sen.Operand[1])) {
    case 0:
        strcpy(inst[0].Sour, "i");
        break;
    case 1:
        strcpy(inst[0].Sour, "r");
        strcpy(inst[0].Word_type, "w");
        break;
    case 2:
        strcpy(inst[0].Sour, "r");
        strcpy(inst[0].Word_type, "b");
        break;
    case 3:
        strcpy(inst[0].Sour, "m");
        break;
    case 4:
        if (!strcmp(inst[0].Opcd, "MOV")) strcpy(inst[0].Sour, "a");
        else strcpy(inst[0].Sour, "r");
        break;
    case 5:
        strcpy(inst[0].Sour, "C");
        return 54;
    case 6:
        strcpy(inst[0].Sour, "D");
        return 55;
    case 7:
        strcpy(inst[0].Sour, "N");
        break;
    default:
        printf("Source_code Error\n");
        return -1;
    }

    i = Instruction_count;

    while (strncmp(inst[i].Opcd, inst[0].Opcd, 6) ||
        strncmp(inst[i].Dest, inst[0].Dest, 2) ||
        strncmp(inst[i].Sour, inst[0].Sour, 2) ||
        strncmp(inst[i].Word_type, inst[0].Word_type, 2)) i--;

    return i;
}

/*
function : to read a source file, then process symbols;
parameters : none;
output : none;
*/
void PassI() {
    char line[51], buf[51];
    int res;
    fp_code = fopen("prog1.txt", "r");
    if (fp_code == NULL) {
        printf("can't open a file");
        exit(1);
    }

    printf("PassI\n");
    while (!feof(fp_code)) {
        fgets(buf, 50, fp_code);
        strcpy(line, buf);
        res = Check_field(line);
        if (res == -1) continue;
        else if (res == 30) {
            printf("%04X: %s", LC, buf);
            LC += 2;
        }
        else if (res >= 26 && res <= 29) {
            printf("%04X: %s", LC, buf);
            LC += 2;
        }
        else if (res) {
            printf("%04X: %s", LC, buf);
            if (sen.Label[0] != '\0') {
                strcpy(symb[Symbol_count].Symbol_name, sen.Label);
                symb[Symbol_count].Location = LC;
                Symbol_count++;
            }
            LC += atoi(inst[res].Length);
        }
        else {
            symb[Symbol_count].Location = LC;
            if (sen.Label[0] != '\0') strcpy(symb[Symbol_count].Symbol_name, sen.Label);
            if (!strncmp(symb[Symbol_count].Word_type, "w", 2)) LC += 2;
            else LC++;
            Symbol_count++;
        }
    }

    fclose(fp_code);

    return;
}

/*
function : to read a source file, then print machine code;
parameters : none;
output : none;
*/
void PassII() {
    char buf[51];
    int res;

    fp_code = fopen("prog1.txt", "r");
    if (fp_code == NULL) {
        printf("can't open a file");
        exit(1);
    }

    LC = 0;
    printf("\nPassII\n");
    while (!feof(fp_code)) {
        char Dest_bits[4], Sour_bits[4], Binary_buf[9];
        int symb_buf;
        fgets(buf, 50, fp_code);
        res = Check_field(buf);
        if (res == -1) continue;
        else if (res == 30) {
            printf("%04X: CD\n", LC);
            printf("%04X: %c%c\n", LC + 1, sen.Operand[0][0], sen.Operand[0][1]);
            LC += 2;
        }
        else if (res >= 26 && res <= 29) {
            for (int i = 0; i < Symbol_count; i++) {
                if (!strcmp(symb[i].Symbol_name, sen.Operand[0])) symb_buf = symb[i].Location;
            }
            switch (res) {
            case 26:
                printf("%04X: D9\n", LC);
                printf("%04X: %02X\n", LC + 1, symb_buf);
                LC += 2;
                break;
            case 27:
                printf("%04X: 77\n", LC);
                printf("%04X: %02X\n", LC + 1, symb_buf);
                LC += 2;
                break;
            case 28:
                printf("%04X: 72\n", LC);
                printf("%04X: %02X\n", LC + 1, symb_buf);
                LC += 2;
                break;
            case 29:
                printf("%04X: 74\n", LC);
                printf("%04X: %02X\n", LC + 1, symb_buf);
                LC++;
                break;
            }
        }
        else if (res) {
            // MOV DS, AX
            if (inst[res].Dest[0] == 'D') {
                printf("%04X: 8E\n", LC);
                printf("%04X: D8\n", LC + 1);
                LC += 2;
            }
            // Dest == acumulator;
            else if (inst[res].Dest[0] == 'a') {
                strcpy(Dest_bits, "000");
                // Sour == CS;
                if (inst[res].Sour[0] == 'C') {
                    printf("%04X: 8C\n", LC);
                    printf("%04X: C8\n", LC + 1);
                    LC += 2;
                }
                // Sour == register;
                if (inst[res].Sour[0] == 'r') {
                    for (int i = 0; i < Register_count; i++) {
                        if (!strcmp(reg[i].Register_name, sen.Operand[1])) strcpy(Sour_bits, reg[i].Register_number);;
                    }
                    strncpy(Binary_buf, inst[res].Mod_reg, 2);
                    strncpy(&Binary_buf[2], Dest_bits, 3);
                    strncpy(&Binary_buf[5], Sour_bits, 3);
                    Binary_buf[8] = '\0';
                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    printf("%04X: %02X\n", LC + 1, btoi(Binary_buf));
                    LC += atoi(inst[res].Length);
                }
                // Sour == immediate;
                else if (inst[res].Sour[0] == 'i') {
                    int i_buf = atoi(sen.Operand[1]);
                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    if (inst[res].Word_type[0] == 'w') {
                        printf("%04X: %02X\n", LC + 1, i_buf);
                        printf("%04X: %02X\n", LC + 2, i_buf >> 8);
                        LC++;
                    }
                    else printf("%04X: %02X\n", LC + 1, i_buf);
                    LC += 2;
                }
                // Sour == memory; (MOV, ADD, SUB)
                else if (inst[res].Sour[0] == 'm') {
                    for (int i = 0; i < Symbol_count; i++) {
                        if (!strcmp(symb[i].Symbol_name, sen.Operand[0])) symb_buf = symb[i].Location;
                    }
                    strncpy(Binary_buf, inst[res].Mod_reg, 2);
                    strncpy(&Binary_buf[2], Dest_bits, 3);
                    strncpy(&Binary_buf[5], "110", 3);
                    Binary_buf[8] = '\0';

                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    printf("%04X: %02X\n", LC + 1, btoi(Binary_buf));
                    printf("%04X: %02X\n", LC + 2, symb_buf);
                    printf("%04X: %02X\n", LC + 3, symb_buf >> 8);
                    LC += 4;
                }
                // Sour == None; (INC, DEC)
                else if (inst[res].Sour[0] == 'N') {
                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    LC++;
                }
            }
            // Dest == register;
            else if (inst[res].Dest[0] == 'r') {
                for (int i = 0; i < 15; i++) {
                    if (sen.Operand[0][0] == R[i][0]) {
                        if (sen.Operand[0][1] == R[i][1]) {
                            strcpy(Dest_bits, reg[i].Register_number);
                            break;
                        }
                    }
                }
                // Sour == register;
                if (inst[res].Sour[0] == 'r') {
                    for (int i = 0; i < Register_count; i++) {
                        if (!strcmp(reg[i].Register_name, sen.Operand[1])) {
                            strcpy(Sour_bits, reg[i].Register_number);
                            break;
                        }
                    }
                    strncpy(Binary_buf, inst[res].Mod_reg, 2);
                    strncpy(&Binary_buf[2], Dest_bits, 3);
                    strncpy(&Binary_buf[5], Sour_bits, 3);
                    Binary_buf[8] = '\0';
                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    printf("%04X: %02X\n", LC + 1, btoi(Binary_buf));
                    LC += atoi(inst[res].Length);
                }
                // Sour == immediate;
                else if (inst[res].Sour[0] == 'i') {
                    if (sen.Operand[1][2] == 'H') {
                        char ch_buf[3];
                        strncpy(ch_buf, sen.Operand[1], 2);
                        ch_buf[2] = '\0';
                        strncpy(Binary_buf, inst[res].Mod_reg, 5);
                        strncpy(&Binary_buf[5], Dest_bits, 3);
                        Binary_buf[8] = '\0';
                        int btoi_buf = btoi(Binary_buf);
                        printf("%04X: %02X\n", LC, btoi_buf);
                        printf("%04X: %s\n", LC + 1, ch_buf);
                    }
                    else {
                        int i_buf = atoi(sen.Operand[1]);
                        strncpy(Binary_buf, inst[res].Mod_reg, 5);
                        strncpy(&Binary_buf[5], Dest_bits, 3);
                        Binary_buf[8] = '\0';
                        int btoi_buf = btoi(Binary_buf);
                        printf("%04X: %s\n", LC, inst[res].Binary_code);
                        printf("%04X: %02X\n", LC + 1, btoi_buf);
                        printf("%04X: %02X\n", LC + 2, i_buf);
                    }
                    LC += 3;
                }
                // Sour == memory; (MOV, ADD, SUB)
                else if (inst[res].Sour[0] == 'm') {
                    for (int i = 0; i < Symbol_count; i++) {
                        if (!strcmp(symb[i].Symbol_name, sen.Operand[1])) {
                            symb_buf = symb[i].Location;
                            break;
                        }
                    }
                    strncpy(Binary_buf, inst[res].Mod_reg, 2);
                    strncpy(&Binary_buf[2], Dest_bits, 3);
                    strncpy(&Binary_buf[5], "110", 3);
                    Binary_buf[8] = '\0';

                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    printf("%04X: %02X\n", LC + 1, btoi(Binary_buf));
                    printf("%04X: %02X\n", LC + 2, symb_buf);
                    printf("%04X: %02X\n", LC + 3, symb_buf >> 8);
                    LC += atoi(inst[res].Length);
                }
                // Sour == None; (INC, DEC)
                else if (inst[res].Sour[0] == 'N') {
                    strncpy(Binary_buf, inst[res].Mod_reg, 5);
                    strncpy(&Binary_buf[5], Dest_bits, 3);
                    Binary_buf[8] = '\0';
                    printf("%04X: %02X\n", LC, btoi(Binary_buf));
                    LC++;
                }
            }
            // Dest == memory;
            else if (inst[res].Dest[0] == 'm') {
                for (int i = 0; i < Symbol_count; i++) {
                    if (!strcmp(symb[i].Symbol_name, sen.Operand[0])) symb_buf = symb[i].Location;
                }
                // Sour == register; (MOV, ADD, SUB, AND, OR, CMP)
                if (inst[res].Sour[0] == 'r') {
                    for (int i = 0; i < Register_count; i++) {
                        if (!strcmp(reg[i].Register_name, sen.Operand[1])) strcpy(Sour_bits, reg[i].Register_number);
                    }
                    strncpy(Binary_buf, inst[res].Mod_reg, sizeof(inst[res].Mod_reg) - 7);
                    strncpy(Binary_buf + 2, Sour_bits, sizeof(Sour_bits) - 1);
                    strncpy(Binary_buf + 5, "110", 3);
                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    printf("%04X: %02X\n", LC + 1, btoi(Binary_buf));
                    printf("%04X: %02X\n", LC + 2, symb_buf);
                    printf("%04X: %02X\n", LC + 3, symb_buf >> 8);
                    LC += atoi(inst[res].Length);
                }
                // Sour == None; (INC, DEC)
                else if (inst[res].Sour[0] == 'N') {
                    strcpy(Binary_buf, inst[res].Mod_reg);
                    printf("%04X: %s\n", LC, inst[res].Binary_code);
                    printf("%04X: %02X\n", LC + 1, btoi(Binary_buf));
                    printf("%04X: %02X\n", LC + 2, symb_buf);
                    printf("%04X: %02X\n", LC + 3, symb_buf >> 8);
                    LC += atoi(inst[res].Length);
                }

            }
            else printf("Error\n");
        }
        else continue;
    }

    fclose(fp_code);

    return;
}


int main() {
    for (int i = 0; i < 20; i++) strcpy(symb[i].Symbol_name, "\0");
    init();
    PassI();
    PassII();

    return 0;
}