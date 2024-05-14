#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    //Error checking - Not enough arguments to output a chart
    if (argc < 2) {
        printf("Not enough arguments\n");
        exit(-1);
    }
    FILE* inFile = fopen(argv[1], "r");
    char originalCode[7][128]; //assumes length of argv[1] is at most 128
    char copyCode[7][128];
    int codeLineCounter = 0;
    int i;
    //Tranfers the code from the inFile line by line to our originalCode character array
    while (fgets(originalCode[codeLineCounter], 128, inFile) != NULL) {
        codeLineCounter++;
    }
    //Copies each line from code array to code2 array
    for (i = 0; i < codeLineCounter; i++) {
        strcpy(copyCode[i], originalCode[i]);
    }
    fclose(inFile);
    char codeInfo[codeLineCounter][4][5]; //RENAME "CODEINFO" TO SOMETHING ELSE 
    //Assigning registers
    for (i = 0; i < codeLineCounter; i++) {
        //char* originalString = copyCode[i];
        char* splitString = strtok(copyCode[i], " ");
        int registerCounter = 0;
        while (splitString != NULL) {
            if (registerCounter == 0) {
                strcpy(codeInfo[i][0], splitString);
                registerCounter++;
            }
            else {
                char* splitString2 = strtok(splitString, ",");
                while (splitString2 != NULL) {
                    strcpy(codeInfo[i][registerCounter], splitString2);
                    splitString2 = strtok(NULL, ",");
                    registerCounter++;
                }
            }
            splitString = strtok(NULL, " ");
        }
    }
    //Simulation Creation
    char* stages[5] = {"IF", "ID", "EX", "MEM", "WB"};
    int stageNumber[7];
    int tabIns[7]; //what is this
    // keeping track of which stage each instruction is at
    for (i = 0; i < codeLineCounter; i++) {
        stageNumber[i] = -1; //tracks which stage the instructions begin at
        tabIns[i] = i; //tracks where to start printing said stage, aka a 'tab-in'
    }
    //Simulation begins [9 stages]
    printf("START OF SIMULATION\n\n");
    for (i = 0; i < 9; i++) {
        printf("CPU Cycles ===>\t1\t2\t3\t4\t5\t6\t7\t8\t9\n");
    }

    printf("END OF SIMULATION\n");
    return EXIT_SUCCESS;
}