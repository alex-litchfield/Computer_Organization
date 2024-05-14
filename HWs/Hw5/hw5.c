#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <ctype.h>

//Finds log base 2 of a given number
int logBase2(int number) {
    int count = 0;
    if (number == 0) {
        return 0;
    }
    while (number != 1)
    {
        if (number % 2 != 0) {
            return 0;
        }
        count++;
        number = number / 2;
    }
    return count;
}

//Checks if the character is an integer constant
bool integerTest(char* c) {
    if (c[0] == '-') {
        return isdigit(c[1]);
    } 
    else {
        return isdigit(c[0]);
    }
}

//Addition operation
void addition(char* c1, char* c2, char* c3) {
    if (integerTest(c2) || integerTest(c3)) {
        printf("addi %s,%s,%s\n", c1, c2, c3);
    } 
    else {
        printf("add %s,%s,%s\n", c1, c2, c3);
    }
}

//Subtraction operation
void subtraction(char* c1, char* c2, char* c3) {
    if (c3[0] == '$') {
        printf("sub %s,%s,%s\n", c1, c2, c3);
    }
    else {
        printf("addi %s,%s,-%s\n", c1, c2, c3);
    }
}

//Multiplication operation
void multiplication(char* c1, char* c2, char* c3, int* tempCounter, char** tempRegisters, int* value) {
    //times 0
    if (c2[0] == '0' || c3[0] == '0') {
        printf("li %s,%d\n", c1, 0);
        return;
    }
    //times negative 1
    else if (c3[0] == '-' && c3[1] == '1' && c3[2] == '\0') {
        printf("move %s,%s\n", tempRegisters[*tempCounter], c2);
        printf("sub %s,$zero,%s\n", c1, tempRegisters[*tempCounter]);
        if (*tempCounter <= 9) {
            (*tempCounter)++;
        } 
        else {
            *tempCounter = 0;
            *value = 1;
        }
        return;
    }
    //times 1
    else if (c3[0] == '1' && c3[1] == '\0') {
        printf("move %s,%s\n", tempRegisters[*tempCounter], c2);
        printf("move %s,%s\n", c1, tempRegisters[*tempCounter]);
        if (*tempCounter <= 9) {
            (*tempCounter)++;
        } 
        else {
            *tempCounter = 0;
            *value = 1;
        }
        return;
    }
    // break into powers of 2
    if (c3[0] == '-' || integerTest(c3)) {
        int temp = atoi(c3);
        int overall = 0;
        int bool_check = 0;
        int holder[30];
        temp = abs(temp);
        while(temp != 0) {
            if (temp % 2 == 1) {
                holder[overall] = 1;
            } 
            else {
                holder[overall] = 0;
            }
            overall++;
            temp = temp / 2;
        }
        int i = overall - 1;
        while (i >= 1) {
            if (bool_check == 0 && holder[i] == 1) {
                printf("sll %s,%s,%d\n", tempRegisters[*tempCounter], c2, i);
                printf("move %s,%s\n", tempRegisters[*tempCounter + 1], tempRegisters[*tempCounter]);
                bool_check = 1;
            } 
            else if (holder[i] == 1){
                printf("sll %s,%s,%d\n", tempRegisters[*tempCounter], c2, i);
                printf("add %s,%s,%s\n", tempRegisters[*tempCounter + 1], tempRegisters[*tempCounter + 1], tempRegisters[*tempCounter]);
            }
            i = i - 1;
        }
        printf("add %s,%s,%s\n", tempRegisters[*tempCounter + 1], tempRegisters[*tempCounter + 1], c2);
        if (c3[0] == '-') {
            if (c1[1] == 't') {
                printf("sub %s,$zero,%s\n", tempRegisters[*tempCounter + 2], tempRegisters[*tempCounter + 1]);
            } 
            else {
                printf("sub %s,$zero,%s\n", c1, tempRegisters[*tempCounter + 1]);
            }
        } 
        else {
            printf("move %s,%s\n", c1, tempRegisters[*tempCounter + 1]);
        }
        *tempCounter = *tempCounter + 3;
        return;
    }
    printf("mult %s,%s\n", c2 ,c3);
    printf("mflo %s\n", c1);
    if (c1[1] == 't') {
        if (*tempCounter >= 10) {
            *tempCounter = 0;
            *value = 1;
        } 
        else {
            (*tempCounter)++;
        }
    }
}

//Division operation
void division(char* c1, char* c2, char* c3, int* tempCounter, char** tempRegisters, int* remainderCounter) {
    //divided by 1
    if (c3[0] == '1' && c3[1] == '\0') {
        printf("move %s,%s\n", c1, c2);
        return;
    }
    //divided by negative 1
    else if (c3[0] == '-' && c3[1] == '1' && c3[2] == '\0') {
        printf("sub %s,$zero,%s\n", c1, c2);
        return;
    }
    //branch when less than zero
    else if (c3[0] == '-' || integerTest(c3)) {
        int temp = atoi(c3);
        int shiftBits = logBase2(abs(temp));
        if (shiftBits > 0) {
            printf("bltz %s,L%d\n", c2, *remainderCounter);
            printf("srl %s,%s,%d\n", c1, c2, shiftBits);
            if (c3[0] == '-') {
                printf("sub %s,$zero,%s\n", c1, c1);
            }
            printf("j L%d\n", *remainderCounter + 1);
            printf("L%d:\n", *remainderCounter);
            printf("li %s,%s\n", tempRegisters[*tempCounter], c3);
            printf("div %s,%s\n", c2, tempRegisters[*tempCounter]);
            printf("mflo %s\n", c1);
            printf("L%d:\n", *remainderCounter + 1);
            (*tempCounter)++;
            *remainderCounter = *remainderCounter + 2;
        }
        else { //branch
            if (c1[1] == 't') {
                printf("li %s,%s\n", c1, c3);
                printf("div %s,%s\n", c2, c1);
                printf("mflo %s\n", tempRegisters[*tempCounter + 1]);
                (*tempCounter)++;
            } 
            else {
                printf("li %s,%s\n", tempRegisters[*tempCounter], c3);
                printf("div %s,%s\n", c2, tempRegisters[*tempCounter]);
                printf("mflo %s\n", c1);
                (*tempCounter)++;
            }
        }
        return;
    }
    printf("div %s,%s\n", c2, c3);
    printf("mflo %s\n", c1);
}

//Modulus
void modulus(char* c1, char* c2, char* c3) {
    printf("div %s,%s\n", c2, c3);
    printf("mfhi %s\n", c1);
}

char* printMIPS(char** registers, int register_amount, int* l_values_amount, char* l_values, int* tempTracker, int* remainderCounter) {
    char* tempRegisters[] = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};
    int value = 0;
    int r = *l_values_amount;
    int t = *tempTracker;
    int LCount = *remainderCounter;
    int i;
    //Putting variables into registers
    for (i = 0; i < register_amount; i++) {
        //if the value is an operator we move on
        if (registers[i][0] == '+' || registers[i][0] == '-' || registers[i][0] == '*' || registers[i][0] == '/' || registers[i][0] == '=' || registers[i][0] == '%') {
            continue;
        }
        //if the value is a number than we move on
        else if (isdigit(registers[i][0]) || registers[i][0] == '$') {
            continue;
        } 
        else {
            (*l_values_amount)++;
            l_values[r] = registers[i][0];
            int j;
            for (j = i + 1; j < register_amount; j++) {
                if(registers[j][0] == registers[i][0]) {
                    sprintf(registers[j], "$s%d", r);
                }
            }
            sprintf(registers[i], "$s%d", r);
            r++;
        }
    }
    //Start printing out the MIPS code
    for (i = 2; i < register_amount; i++) {
        //addition
        if (i + 1 < register_amount && registers[i + 1][0] == '+') {
            if (i + 4 < register_amount && t == 0 && value == 1) {
                addition(tempRegisters[t], tempRegisters[9], registers[i + 2]);
                t++;
            } 
            else  if (i + 4 < register_amount && t != 0) {
                addition(tempRegisters[t], tempRegisters[t - 1], registers[i + 2]);
                t++;
            } 
            else if (i + 4 < register_amount && t == 0) {
                addition(tempRegisters[t], registers[i], registers[i + 2]);
                t++;
            } 
            else if (t == 0) {
                addition(registers[0], registers[i], registers[i + 2]);
            } 
            else {
                addition(registers[0], tempRegisters[t - 1], registers[i + 2]);
            }
        }
        //subtraction
        if (i + 1 < register_amount && registers[i + 1][0] == '-' && registers[i + 1][1] == '\0') {
            if (i + 4 < register_amount && t == 0 && value == 1) {
                subtraction(tempRegisters[t], tempRegisters[9], registers[i + 2]);
                t++;
            } 
            else  if (i + 4 < register_amount && t != 0) {
                subtraction(tempRegisters[t], tempRegisters[t - 1], registers[i + 2]);
                t++;
            } 
            else if (i + 4 < register_amount && t == 0) {
                subtraction(tempRegisters[t], registers[i], registers[i + 2]);
                t++;
            } 
            else if (t == 0) {
                subtraction(registers[0], registers[i], registers[i + 2]);
            } 
            else {
                subtraction(registers[0], tempRegisters[t - 1], registers[i + 2]);
            }
        }
        //multiplication
        if (i + 1 < register_amount && registers[i + 1][0] == '*') {
            if (i == 2 && t != 0 && i + 4 < register_amount) {
                multiplication(tempRegisters[t], registers[i], registers[i + 2], &t, tempRegisters, &value);
            } 
            else if (i + 4 < register_amount && t == 0 && value == 1) {
                multiplication(tempRegisters[t], tempRegisters[9], registers[i + 2], &t, tempRegisters, &value);
            } 
            else if (i + 4 > register_amount && t == 0 && value == 1) {
                multiplication(registers[0], tempRegisters[9], registers[i + 2], &t, tempRegisters, &value);
            } 
            else if (i + 4 < register_amount && t == 0) {
                multiplication(tempRegisters[t], registers[i], registers[i + 2], &t, tempRegisters, &value);
            } 
            else  if (i + 4 < register_amount && t != 0) {
                multiplication(tempRegisters[t], tempRegisters[t - 1], registers[i + 2], &t, tempRegisters, &value);
            } 
            else if (i == 2 && t != 0) {
                multiplication(registers[0], registers[i], registers[i + 2], &t, tempRegisters, &value);
            }
            else if (t == 0) {
                multiplication(registers[0], registers[i], registers[i + 2], &t, tempRegisters, &value);
            } 
            else {
                multiplication(registers[0], tempRegisters[t - 1], registers[i + 2], &t, tempRegisters, &value);
            }
        }
        //division
        if (i + 1 < register_amount && registers[i + 1][0] == '/') {
            if (i + 4 < register_amount && t == 0 && value == 1) {
                division(tempRegisters[t], tempRegisters[9], registers[i + 2], &t, tempRegisters, &LCount);
                t++;
            } 
            else  if (i + 4 < register_amount && t != 0) {
                division(tempRegisters[t], tempRegisters[t - 1], registers[i + 2], &t, tempRegisters, &LCount);
                t++;
            } 
            else if (i + 4 < register_amount && t == 0) {
                division(tempRegisters[t], registers[i], registers[i + 2], &t, tempRegisters, &LCount);
                t++;
            } 
            else if (t == 0) {
                division(registers[0], registers[i], registers[i + 2], &t, tempRegisters, &LCount);
            } 
            else {
                division(registers[0], tempRegisters[t - 1], registers[i + 2], &t, tempRegisters, &LCount);
            }
        }
        //modulus
        if (i + 1 < register_amount && registers[i + 1][0] == '%') {
            if (i + 4 < register_amount && t == 0 && value == 1) {
                modulus(tempRegisters[t], tempRegisters[9], registers[i + 2]);
                t++;
            } 
            else if (i + 4 < register_amount && t != 0) {
                modulus(tempRegisters[t], tempRegisters[t - 1], registers[i + 2]);
                t++;
            } 
            else if (i + 4 < register_amount && t == 0) {
                modulus(tempRegisters[t], registers[i], registers[i + 2]);
                t++;
            }
            else if (t == 0) {
                modulus(registers[0], registers[i], registers[i + 2]);
            } 
            else {
                modulus(registers[0], tempRegisters[t - 1], registers[i + 2]);
            }
        }
        if (t >= 10) {
            value = 1;
            t = 0;
        }
        i++;
    }
    *tempTracker = t;
    *remainderCounter = LCount;
    return l_values;
}

int main(int argc, char * argv[]) {
    char * current_line = NULL;
    int current_register = 0;
    int l_values_amount = 0;
    int register_number = 0;
    int register_amount = 0;
    int remainderCounter = 0;
    int tempTracker = 0;
    size_t buffer_size = 0;
    //holds C references (lvalues) to use with MIPS registers
    char* l_values = calloc((128), sizeof(char ));
    //Opening file
    FILE* infile = fopen(argv[1], "r");
    if (infile == NULL) {
        printf("mm_read: failed to open file.\n");
        exit(-1);
    }
    //Allocating space for 2D array
    char** registers = (char**) calloc((128), sizeof(char*));
    for (int i = 0; i < 128; i++) {
        registers[i] = (char*) calloc((8), sizeof(char));
    }
    //Reads in text one line at a time
    while (getline(&current_line, &buffer_size, infile) != -1) {
        printf("# %s", current_line);
        register_amount = 0; //this was size
        current_register  = 0; // this was r
        
        //split the string by spaces
        char* split_string = strtok(current_line, " ");
        while (split_string != NULL) {
            strcpy(registers[current_register], split_string);
            split_string = strtok (NULL, " ;\n");
            current_register++;
            register_amount++;
        }

        //When register_amount is 3
        if (register_amount == 3) {
            printf("li $s%d,%s\n", register_number, registers[2]);
            l_values[l_values_amount] = registers[0][0];
            l_values_amount++;
            register_number++;
        }
        //When register_amount is not 3
        else {
            for (int i = 0; i < register_amount; i++) {
                for (int k = 0; k < l_values_amount; k++) {
                    if (registers[i][0] == l_values[k]) {
                        sprintf(registers[i], "$s%d", k);
                    }
                }
            }
            l_values = printMIPS(registers, register_amount, &l_values_amount, l_values, &tempTracker, &remainderCounter);
        }
    }
    //Closing files and cleaning memory
    fclose(infile);
    free(l_values);
    free(registers);
    for (int i = 0; i < 128; i++) {
        free(registers[i]);
    }
    return 0;
}