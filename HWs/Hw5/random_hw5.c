#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


//checks if it is a pwoer of two
int isPowerOfTwo(int n)
{
    int count = 0;
    if (n == 0)
        return 0;
    while (n != 1)
    {
        if (n%2 != 0)
            return 0;
        n = n/2;
        count++;
    }
    return count;
}


void add(char* a, char* b, char* c) {
    printf("add %s,%s,%s\n", a ,b ,c);
}

void sub(char* a, char* b, char* c) {
    printf("sub %s,%s,%s\n", a ,b ,c);
}

void divide(char* a, char* b, char* c, int* t, char** tempt, int* LCount) {
    if (c[0] == '1' && c[1] == '\0') {
        printf("move %s,%s\n", a, b);
        return;
    } else if (c[0] == '-' && c[1] == '1' && c[2] == '\0') {
        printf("sub %s,$zero,%s\n", a, b);
        return;
    } else if (c[0] == '-' || isdigit(c[0])) {
        int temp = atoi(c);
        int srlV;
        if ((srlV = isPowerOfTwo(abs(temp))) > 0) {
            printf("bltz %s,L%d\n", b, *LCount);
            printf("srl %s,%s,%d\n", a, b, srlV);
            if (c[0] == '-') {
                printf("sub %s,$zero,%s\n", a, a);
            }
            printf("j L%d\n", *LCount + 1);
            printf("L%d:\n", *LCount);
            printf("li %s,%s\n", tempt[*t], c);
            printf("div %s,%s\n", b, tempt[*t]);
            printf("mflo %s\n", a);
            printf("L%d:\n", *LCount + 1);
            *LCount += 2;
            (*t)++;
        } else {
            if (a[1] == 't') {
                printf("li %s,%s\n", a, c);
                printf("div %s,%s\n", b, a);
                printf("mflo %s\n", tempt[*t + 1]);
                (*t)++;
            } else {
                printf("li %s,%s\n", tempt[*t], c);
                printf("div %s,%s\n", b, tempt[*t]);
                printf("mflo %s\n", a);
                (*t)++;
            }
        }
        return;
    }
    printf("div %s,%s\n", b ,c);
    printf("mflo %s\n", a);
}

void mult(char* a, char* b, char* c, int* t, char** tempt, int* value) {
    if (b[0] == '0' || c[0] == '0') {
        printf("li %s,%d\n", a ,0);
        return;
    } else if (c[0] == '1' && c[1] == '\0') {
        printf("move %s,%s\n", tempt[*t], b);
        printf("move %s,%s\n", a, tempt[*t]);
        if (*t >= 10) {
            *value = 1;
            *t = 0;
        } else {
            (*t)++;
        }
        return;
    } else if (c[0] == '-' && c[1] == '1' && c[2] == '\0') {
        printf("move %s,%s\n", tempt[*t], b);
        printf("sub %s,$zero,%s\n", a, tempt[*t]);
        if (*t >= 10) {
            *value = 1;
            *t = 0;
        } else {
            (*t)++;
        }
        return;
    } if (c[0] == '-' || isdigit(c[0])) {
        //have to build out of sll
        int temp = atoi(c);
        temp = abs(temp);
        int total = 0;
        int bin[30];
        int check = 0;
        while(temp != 0) {
            if (temp % 2 == 1) {
                bin[total] = 1;
            } else {
                bin[total] = 0;
            }
            temp = temp / 2;
            total++;
        }
        for (int i = total - 1; i >= 1; i--) {
            if (check == 0 && bin[i] == 1) {
                printf("sll %s,%s,%d\n", tempt[*t], b, i);
                printf("move %s,%s\n", tempt[*t + 1], tempt[*t]);
                check = 1;
            } else if (bin[i] == 1){
                printf("sll %s,%s,%d\n", tempt[*t], b, i);
                printf("add %s,%s,%s\n", tempt[*t + 1], tempt[*t + 1], tempt[*t]);
            }
        }
        printf("add %s,%s,%s\n", tempt[*t + 1], tempt[*t + 1], b);
        if (c[0] == '-') {
            if (a[1] == 't') {
                printf("sub %s,$zero,%s\n", tempt[*t + 2],tempt[*t + 1]);
            } else {
                printf("sub %s,$zero,%s\n", a,tempt[*t + 1]);
            }
        } else {
            printf("move %s,%s\n", a,tempt[*t + 1]);
        }
        *t += 3;
        return;
    }
    printf("mult %s,%s\n", b ,c);
    printf("mflo %s\n", a);
    if (a[1] == 't') {
        if (*t >= 10) {
            *value = 1;
            *t = 0;
        } else {
            (*t)++;
        }
    }
}

void mod(char* a, char* b, char* c) {
    printf("div %s,%s\n", b ,c);
    printf("mfhi %s\n", a);
}

void regLoad(char** line, int regCount) {
    printf("# %s %s %s;\n", line[0], line[1], line[2]);
    printf("li $s%d,%s\n", regCount, line[2]);
}


char* printFunct(char** reg, int size, int* countHold, char* cHold, int* tCounter, int* lc) {
    int r = *countHold;
    
    //load the regestiers
    for (int i = 0; i < size; i++) {
        if(reg[i][0] == '=' || reg[i][0] == '+' || reg[i][0] == '-' || reg[i][0] == '*' || reg[i][0] == '/' || reg[i][0] == '%') { //skip if not a variable
            continue;
        } else if(isdigit(reg[i][0]) || reg[i][0] == '$') { //skip if a number
            continue;
        } else {
            cHold[r] = reg[i][0];
            (*countHold)++;
            for (int k = i + 1; k < size; k++) {
                if(reg[k][0] == reg[i][0]) {
                    sprintf(reg[k], "$s%d", r);
                }
            }
            sprintf(reg[i], "$s%d", r);
            r++;
        }
    }
    //print time
    char* tempt[] = { "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9" };
    int t = *tCounter;
    int LCount = *lc;
    int value = 0;
    
    for (int i = 2; i < size; i++) {
        if (i + 1 < size) {
            if (reg[i + 1][0] == '-' && reg[i + 1][1] == '\0') {
                if (i + 4 < size && t == 0 && value == 1) {
                    sub(tempt[t],tempt[9], reg[i + 2]);
                    t++;
                } else if (i + 4 < size && t == 0) {
                    sub(tempt[t],reg[i], reg[i + 2]);
                    t++;
                } else  if (i + 4 < size && t != 0) {
                    sub(tempt[t],tempt[t - 1], reg[i + 2]);
                    t++;
                } else if (t == 0) {
                    sub(reg[0],reg[i], reg[i + 2]);
                } else {
                    sub(reg[0],tempt[t - 1], reg[i + 2]);
                }
            }
            if (reg[i + 1][0] == '+') {
                if (i + 4 < size && t == 0 && value == 1) {
                    add(tempt[t],tempt[9], reg[i + 2]);
                    t++;
                } else if (i + 4 < size && t == 0) {
                    add(tempt[t],reg[i], reg[i + 2]);
                    t++;
                } else  if (i + 4 < size && t != 0) {
                    add(tempt[t],tempt[t - 1], reg[i + 2]);
                    t++;
                } else if (t == 0) {
                    add(reg[0],reg[i], reg[i + 2]);
                } else {
                    add(reg[0],tempt[t - 1], reg[i + 2]);
                }
            }
            if (reg[i + 1][0] == '/') {
                if (i + 4 < size && t == 0 && value == 1) {
                    divide(tempt[t],tempt[9], reg[i + 2], &t, tempt, &LCount);
                    t++;
                } else if (i + 4 < size && t == 0) {
                    divide(tempt[t],reg[i], reg[i + 2], &t, tempt, &LCount);
                    t++;
                } else  if (i + 4 < size && t != 0) {
                    divide(tempt[t],tempt[t - 1], reg[i + 2], &t, tempt, &LCount);
                    t++;
                } else if (t == 0) {
                    divide(reg[0],reg[i], reg[i + 2], &t, tempt, &LCount);
                } else {
                    divide(reg[0],tempt[t - 1], reg[i + 2], &t, tempt, &LCount);
                }
            }
            if (reg[i + 1][0] == '*') {
                if (i == 2 && t != 0 && i + 4 < size) { //edge check later
                    mult(tempt[t],reg[i], reg[i + 2], &t, tempt, &value);
                } else if (i == 2 && t != 0) {
                    mult(reg[0],reg[i], reg[i + 2], &t, tempt, &value);
                }else if (i + 4 < size && t == 0 && value == 1) {
                    mult(tempt[t],tempt[9], reg[i + 2], &t, tempt, &value);
                } else if (i + 4 > size && t == 0 && value == 1) {
                    mult(reg[0],tempt[9], reg[i + 2], &t, tempt, &value);
                } else if (i + 4 < size && t == 0) {
                    mult(tempt[t],reg[i], reg[i + 2], &t, tempt, &value);
                } else  if (i + 4 < size && t != 0) {
                    mult(tempt[t],tempt[t - 1], reg[i + 2], &t, tempt, &value);
                } else if (t == 0) {
                    mult(reg[0],reg[i], reg[i + 2], &t, tempt, &value);
                } else {
                    mult(reg[0],tempt[t - 1], reg[i + 2], &t, tempt, &value);
                }
            }
            if (reg[i + 1][0] == '%') {
                if (i + 4 < size && t == 0 && value == 1) {
                    mod(tempt[t],tempt[9], reg[i + 2]);
                    t++;
                } else if (i + 4 < size && t == 0) {
                    mod(tempt[t],reg[i], reg[i + 2]);
                    t++;
                } else  if (i + 4 < size && t != 0) {
                    mod(tempt[t],tempt[t - 1], reg[i + 2]);
                    t++;
                } else if (t == 0) {
                    mod(reg[0],reg[i], reg[i + 2]);
                } else {
                    mod(reg[0],tempt[t - 1], reg[i + 2]);
                }
            }
        }
        if (t >= 10) {
            value = 1;
            t = 0;
        }
    }
    *lc = LCount;
    *tCounter = t;
    return cHold;
}

int main( int argc, char * argv[] )
{
    int r = 0;
    int size = 0;
    FILE* fp;
    char* line= NULL;
    size_t len = 0;
    int regCount = 0;
    int t = 0;
    int lc = 0;
    char* cHold = calloc((128), sizeof(char ));
    int countHold = 0;
    
    char** reg = (char**) calloc((128), sizeof(char*));
    for (int i = 0; i < 128; i++) {
        reg[i] = (char*) calloc((5), sizeof(char));
    }
    
    
    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    
    while (getline(&line, &len, fp) != -1) {
        size = 0;
        r  = 0;
        
        char* pch = strtok(line, " ");
        while (pch != NULL) {
            strcpy(reg[r], pch);
            size++;
            pch = strtok (NULL, " ;\n");
            r++;
        }
        
        if (size == 3) {
            regLoad(reg, regCount);
            cHold[countHold] = reg[0][0];
            countHold++;
            regCount++;
        } else {
            printf("#");
            for (int i = 0; i < size; i++) {
                printf(" %s", reg[i]);
            }
            printf(";\n");
            for (int i = 0; i < size; i++) {
                for (int k = 0; k < countHold; k++) {
                    if (reg[i][0] == cHold[k]) {
                        sprintf(reg[i], "$s%d", k);
                    }
                }
            }
            cHold = printFunct(reg, size, &countHold, cHold, &t, &lc);
        }
    }
    
    for (int i = 0; i < 128; i++) {
        free(reg[i]);
    }
    free(reg);
    free(cHold);
    fclose(fp);
    return EXIT_SUCCESS;
}

