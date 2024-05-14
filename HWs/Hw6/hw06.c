#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Instruction {
    char operation[5];
    char a[5];
    char part1[5];
    char part2[5];
    char code[128];
} default_instruction = {.operation = "\0", .a = "\0", .part1 = "\0", .part2 = "\0", .code = "\0"};

struct Line {
    struct Instruction instructions;
    char columns[9][5];
    int start_spot;
    int nop_spot;
    int stage;
};

//Global Variables
struct Line table[128];
char nop1Stages[5][4] = {"IF", "ID", "*", "*", "*"};
char nop2Stages[6][4] = {"IF", "ID", "ID", "*", "*", "*"};
char stages[5][4] = {"IF", "ID", "EX", "MEM", "WB"};
int cycleNumber = 0;
int executionLine = 0;
int currentLine = 0;

int depCompare(struct Instruction *i, char *part1, char *part2) {
    return (part1[0] != '\0' && strcmp(i->a, part1) == 0) || (part2[0] != '\0' && strcmp(i->a, part2) == 0);
}

int depPresent(struct Instruction *i, char *d) {
    return d[0] != '\0' && (((i->part1)[0] != '\0' && strcmp(i->part1, d) == 0) || ((i->part2)[0]!= '\0' && strcmp(i->part2, d) == 0));
}

int nop1Present(struct Line *l) {
    return strcmp(l->instructions.operation, "nop1") == 0;
}

int nop2Present(struct Line *l) {
    return strcmp(l->instructions.operation, "nop2") == 0;
}

void print_all() {
    printf("CPU Cycles ===>\t1\t2\t3\t4\t5\t6\t7\t8\t9\n");
    int i;
    for (i = 0; i < currentLine; i++) {
        if (nop1Present(&table[i]) && cycleNumber > table[i].start_spot + 1) {
            printf("nop\t\t");
        } 
        else if (nop2Present(&table[i]) && cycleNumber > table[i].start_spot + 2) {
            printf("nop\t\t");
        } 
        else {
            printf("%s\t", table[i].instructions.code);
        }
        int j;
        for (j = 0; j < 8; j++) {
            printf("%s\t", table[i].columns[j]);
        }
        printf("%s\n", table[i].columns[8]);
    }
    printf("\n");
}

void readInParts(const char *current_line, int *i, char *a, int strLen) {
    int j;
    for (j = *i; j < strLen; j++) {
        if (current_line[j] == '$') {
            a[0] = current_line[j + 1];
            a[1] = current_line[j + 2];
            a[2] = '\0';
            j = j + 3;
            break;
        }
    }
    *i = j;
}

int main(int argc, char *argv[]) {
    //checks if enough parameters are present
    if (argc < 2) {
        printf("Missing Parameters\n");
        exit(-1);
    }
    FILE *inFile = fopen(argv[1], "r");
    int i;
    for (i = 0; i < 128; i++) {
        table[i].instructions = default_instruction;
        table[i].stage = 0;
        table[i].start_spot = 0;
        table[i].nop_spot = -1;
        int j;
        for (j = 0; j < 9; ++j) {
            strcpy(table[i].columns[j], ".");
        }
    }

    //Checking Dependecy
    char current_line[128];
    while (fgets(current_line, 128, inFile)) {
        current_line[strcspn(current_line, "\n")] = '\0';
        int strLen = strlen(current_line);
        int i = 0;
        int indexNumber = 0;
        int j;
        char a[5];
        char inst[5];
        char part1[5];
        char part2[5];
        for (j = i; j < strLen; j++) {
            if (current_line[j] != ' ') {
                inst[indexNumber] = current_line[j];
                indexNumber++;
            } 
            else {
                inst[indexNumber] = '\0';
                i = j + 1;
                break;
            }
        }
        //Figures out type of instruction
        if (strcmp(inst, "or") == 0 || strcmp(inst, "sub") == 0 || strcmp(inst, "add") == 0 || strcmp(inst, "and") == 0) {
            readInParts(current_line, &i, a, strLen);
            readInParts(current_line, &i, part1, strLen);
            readInParts(current_line, &i, part2, strLen);
        } 
        else if (strcmp(inst, "andi") == 0 || strcmp(inst, "ori") == 0 || strcmp(inst, "addi") == 0 || strcmp(inst, "lw") == 0) {
            readInParts(current_line, &i, a, strLen);
            readInParts(current_line, &i, part1, strLen);
        } 
        else if (strcmp(inst, "sw") == 0) {
            readInParts(current_line, &i, part1, strLen);
            readInParts(current_line, &i, part2, strLen);
        } 
        else {
            printf("Unknown Reg type: '%s'\n", inst);
        } 
        int nopsPosition = -1;
        for (j = currentLine - 1; j >= 0; j--) {
            if (!(nop1Present(&table[j]) && nop2Present(&table[j]))) {
                //Detect if a dependecy is present
                if (depCompare(&table[j].instructions, part1, part2)) {
                    //Fine distance from original spot
                    int dist = currentLine - table[j].start_spot;
                    if (dist > 0 && dist < 3) {
                        if (j == currentLine - 1 || !depPresent(&table[currentLine - 1].instructions, table[j].instructions.a)) {
                            nopsPosition = j;
                            strcpy(table[currentLine].instructions.operation, "nop1");
                            table[currentLine].start_spot = executionLine;
                            currentLine++;
                            if (dist == 1) {
                                strcpy(table[currentLine].instructions.operation, "nop2");
                                table[currentLine].start_spot = executionLine;
                                currentLine++;
                            }
                        }
                    }
                    break; //Dependecy has been found at this point
                }
            }
        }
        //Making the current line
        strcpy(table[currentLine].instructions.code, current_line);
        strcpy(table[currentLine].instructions.operation, inst);
        strcpy(table[currentLine].instructions.part1, part1);
        strcpy(table[currentLine].instructions.part2, part2);
        strcpy(table[currentLine].instructions.a, a);
        table[currentLine].nop_spot = nopsPosition;
        table[currentLine].start_spot = executionLine;
        executionLine++;
        currentLine++;
    }
    fclose(inFile);
    //Starting simulation
    printf("START OF SIMULATION\n\n");
    while (cycleNumber < 9) {
        if (table[0].stage < 5) {
            strcpy(table[0].columns[cycleNumber], stages[table[0].stage]);
            table[0].stage++;
        }
        int i;
        for (i = 1; i < currentLine; i++) {
            if (cycleNumber >= table[i].start_spot) {
                if (nop2Present(&table[i]) && table[i].stage < 6) {
                    strcpy(table[i].columns[cycleNumber], nop2Stages[table[i].stage]);
                    table[i].stage++;
                } 
                else if (nop1Present(&table[i]) && table[i].stage < 5) {
                    strcpy(table[i].columns[cycleNumber], nop1Stages[table[i].stage]);
                    table[i].stage++;
                } 
                else {
                    if (table[i].stage < 5) {
                        // Check dep
                        if (table[i].nop_spot != -1) {
                            if (table[table[i].nop_spot].stage != 5 && table[i].stage == 1) {
                                strcpy(table[i].columns[cycleNumber], stages[table[i].stage]);
                            } 
                            else {
                                strcpy(table[i].columns[cycleNumber], stages[table[i].stage]);
                                table[i].stage++;
                            }
                        } 
                        else {
                            strcpy(table[i].columns[cycleNumber], stages[table[i].stage]);
                            if (table[i - 1].stage == 5 || table[i - 1].stage - table[i].stage > 1) {
                                // If the previous block, wait for it
                                table[i].stage++;
                            }
                        }
                    }
                }
            }
        }
        if (table[currentLine - 1].stage == 5) {
            print_all();
            break;
        }
        print_all();
        cycleNumber++;
    }
    printf("END OF SIMULATION");
    return EXIT_SUCCESS;
}