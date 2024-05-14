#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

void insertBubble(char code[7][128], unsigned int numCodeLines, int hazardInstructionIndex, int stagesTracker[7], unsigned int offset[7]) {
    // update code with nop insertion
    char codeCopy[7][128];
    char codeCopy2[7][128];
    for (unsigned int i = 0; i < numCodeLines; i++) {
        strcpy(codeCopy[i], code[i]);
        strcpy(codeCopy2[i], code[i]);
    }
    for (unsigned int i = hazardInstructionIndex + 1; i <= numCodeLines; i++) {
        strcpy(codeCopy[i], codeCopy2[i - 1]);
    }
    strcpy(codeCopy[hazardInstructionIndex], "nop\t\t.\n");
    for (unsigned int i = 0; i <= numCodeLines; i++) {
        strcpy(code[i], codeCopy[i]);
    }
    // update stagesTracker with nop insertion
    int stagesTrackerCopy[7];
    for (unsigned int i = 0; i < numCodeLines; i++) {
        stagesTrackerCopy[i] = stagesTracker[i];
    }
    stagesTrackerCopy[hazardInstructionIndex] = 1;
    for (unsigned int i = hazardInstructionIndex + 1; i <= numCodeLines; i++) {
        stagesTrackerCopy[i] = stagesTracker[i - 1];
    }
    for (unsigned int i = 0; i <= numCodeLines; i++) {
        stagesTracker[i] = stagesTrackerCopy[i];
    }
    // update offset with nop insertion
    unsigned int offsetCopy[7];
    for (unsigned int i = 0; i < numCodeLines; i++) {
        offsetCopy[i] = offset[i];
    }
    for (unsigned int i = hazardInstructionIndex + 1; i <= numCodeLines; i++) {
        offsetCopy[i] = offset[i - 1];
    }
    offsetCopy[hazardInstructionIndex] = offset[hazardInstructionIndex - 1];
    for (unsigned int i = 0; i <= numCodeLines; i++) {
        offset[i] = offsetCopy[i];
    }
}

void printCode(unsigned int numCodeLines, char code[7][128], int stagesTracker[7], unsigned int offset[7], char* stages[5], bool nopAlert) {
    for (unsigned int j = 0; j < numCodeLines; j++) {
        unsigned int remainingDots = 9;
        // printing mips code
        for (unsigned int k = 0; k < 128; k++) {
            if (code[j][k] == '\n') {
                break;
            } else {
                printf("%c", code[j][k]);
            }
        }
        // stage at which code is at
        int stageIndex = stagesTracker[j];
        if (stageIndex == -1) {
            for (unsigned int k = 0; k < remainingDots; k++) {
                printf("\t.");
            }
            printf("\n");
        } else {
            for (unsigned int k = 0; k < offset[j]; k++) {
                printf("\t.");
                remainingDots--;
            }
            int nopCounter = 0;
            for (unsigned int i = 0; i < numCodeLines; i++) {
                if (code[i][0] == 'n') {
                    nopCounter++;
                }
            }
            int stageIndexCopy = stageIndex;
            for (int k = 0; k <= stageIndex; k++) {
                    printf("\t");
                if (k < 2 || code[j][0] != 'n') {
                    if (nopAlert) {
                        if (k == 1) {
                            for (unsigned int y = 0; y <= nopCounter; y++) {
                                printf("%s", stages[k]);
                                if (y != nopCounter) {
                                    printf("\t");
                                }
                            }
                            stageIndex--;
                        } else {
                            printf("%s", stages[k]);
                        }
                    } else {
                        printf("%s", stages[k]);
                    }
                } else {
                    printf("*");
                    nopAlert = true;
                }
            }
            int stageIndex = stageIndexCopy;
            remainingDots -= (stageIndex + 1);
            if (code[j][0] == 'n') {
                remainingDots--;
            }
            for (unsigned int k = 0; k < remainingDots; k++) {
                printf("\t.");
            }
            printf("\n");
        }
    }
}

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments\n");
        return EXIT_FAILURE;
    }
    FILE* my_file = fopen(argv[1], "r");
    char code[7][128];
    char code2[7][128];
    unsigned int numCodeLines = 0;
    // reading mips code from file
    while (fgets(code[numCodeLines], 128, my_file) != NULL) {
        numCodeLines++;
    }
    fclose(my_file);
    for (unsigned int i = 0; i < numCodeLines; i++) {
        strcpy(code2[i], code[i]);
    }
    char codeInformation[numCodeLines][4][5];
    
    // gather registers
    for (unsigned int i = 0; i < numCodeLines; i++) {
        char* buffer = code2[i];
        char* split = strtok(buffer, " ");
        int counter = 0;
        while (split != NULL) {
            if (counter == 0) {
                strcpy(codeInformation[i][0], split);
                counter++;
            } else {
                char* split2 = strtok(split, ",");
                while (split2 != NULL) {
                    strcpy(codeInformation[i][counter], split2);
                    counter++;
                    split2 = strtok(NULL, ",");
                }
            }
            split = strtok(NULL, " ");
        }
    }
    
    // setting up simulation
    char* header = "CPU Cycles ===>\t1\t2\t3\t4\t5\t6\t7\t8\t9\n";
    char* stages[5];
    stages[0] = "IF";
    stages[1] = "ID";
    stages[2] = "EX";
    stages[3] = "MEM";
    stages[4] = "WB";
    int stagesTracker[7];
    unsigned int offset[7];
    // keeping track of which stage each instruction is at
    for (unsigned int i = 0; i < numCodeLines; i++) {
        stagesTracker[i] = -1;
    }
    // keeping track of where to start printing the stages
    for (unsigned int i = 0; i < numCodeLines; i++) {
        offset[i] = i;
    }
    
    printf("START OF SIMULATION\n\n");
    bool oneBubble = false;
    // simulating 9 CPU cycles
    for (unsigned int i = 0; i < 9; i++) {
        bool nopAlert = false;
        int hazardInstructionIndex = -1;
        printf("%s", header);
        // checking for data hazards
        for (unsigned int m = 1; m < numCodeLines; m++) {
            if (stagesTracker[m] == 1 && strcmp(codeInformation[m][0], "nop") != 0) {
                hazardInstructionIndex = m;
                char rvalue1[256];
                if (codeInformation[m][2][0] == '$') {
                    strcpy(rvalue1, codeInformation[m][2]);
                } else {
                    for (unsigned int z = 0; z < strlen(codeInformation[m][2]); z++) {
                        if (codeInformation[m][2][z] == '$') {
                            rvalue1[0] = codeInformation[m][2][z];
                            rvalue1[1] = codeInformation[m][2][z + 1];
                            rvalue1[2] = codeInformation[m][2][z + 2];
                        }
                    }
                }
                char rvalue2[256];
                bool rvalue2Exists = false;
                if (strcmp(codeInformation[m][0], "add") == 0 || strcmp(codeInformation[m][0], "sub") == 0 || strcmp(codeInformation[m][0], "and") == 0 || strcmp(codeInformation[m][0], "or") == 0) {
                    if (codeInformation[m][3][0] == '$') {
                        strcpy(rvalue2, codeInformation[m][3]);
                    } else {
                        for (unsigned int z = 0; z < strlen(codeInformation[m][3]); z++) {
                            if (codeInformation[m][3][z] == '$') {
                                rvalue2[0] = codeInformation[m][3][z];
                                rvalue2[1] = codeInformation[m][3][z + 1];
                                rvalue2[2] = codeInformation[m][3][z + 2];
                            }
                        }
                    }
                    rvalue2Exists = true;
                }
                char* lvalues[4];
                int lvaluesCounter = 0;
                for (unsigned int l = 0; l < m; l++) {
                    if (strcmp(codeInformation[l][0], "nop") != 0) {
                        if (codeInformation[l][1][0] == '$') {
                            lvalues[l] = codeInformation[l][1];
                            lvaluesCounter++;
                        } else {
                            for (unsigned int z = 0; z < strlen(codeInformation[l][1]); z++) {
                                if (codeInformation[l][1][z] == '$') {
                                    lvalues[l][0] = codeInformation[l][1][z];
                                    lvalues[l][1] = codeInformation[l][1][z + 1];
                                    lvalues[l][2] = codeInformation[l][1][z + 2];;
                                    lvaluesCounter++;
                                }
                            }
                        }
                    }
                }
                for (unsigned int n = 0; n < lvaluesCounter; n++) {
                    if (strcmp(lvalues[n], rvalue1) == 0 && stagesTracker[n] != 4 && !oneBubble) {
                        insertBubble(code, numCodeLines, hazardInstructionIndex, stagesTracker, offset);
                        numCodeLines++;
                        oneBubble = true;
                    } else {
                        if (rvalue2Exists) {
                            if (strcmp(lvalues[n], rvalue2) == 0 && stagesTracker[n] != 4 && !oneBubble) {
                                insertBubble(code, numCodeLines, hazardInstructionIndex, stagesTracker, offset);
                                numCodeLines++;
                                oneBubble = true;
                            }
                        }
                    }
                }
            }
        }
        // updating stages each instruction is at
        int totalNops = 0;
        for (unsigned int i = 0; i < numCodeLines; i++) {
            if (code[i][0] == 'n') {
                totalNops++;
            }
        }
        int latestNop = -1;
        for (unsigned int i = 0; i < numCodeLines; i++) {
            if (code[i][0] == 'n') {
                latestNop = i;
            } else {
                if (latestNop != -1 && i > latestNop) {
                    if (stagesTracker[i] == 4 && stagesTracker[i] < 4 + totalNops) {
                        stagesTracker[i]++;
                    }
                }
            }
        }
        if (i + 1 < numCodeLines) {
            for (unsigned int k = 0; k <= i; k++) {
                if (stagesTracker[k] < 4) {
                    stagesTracker[k]++;
                }
            }
        } else {
            for (unsigned int k = 0; k < numCodeLines; k++) {
                if (stagesTracker[k] < 4) {
                    stagesTracker[k]++;
                }
            }
        }
        printCode(numCodeLines, code, stagesTracker, offset, stages, nopAlert);
        printf("\n");
        int nopCounter2 = 0;
        for (unsigned int i = 0; i < numCodeLines; i++) {
            if (code[i][0] == 'n') {
                nopCounter2++;
            }
        }
        if (stagesTracker[numCodeLines - 1] == 4 + nopCounter2) {
            break;
        }
    }
    printf("END OF SIMULATION\n");
    
    return EXIT_SUCCESS;
}
