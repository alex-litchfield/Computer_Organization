#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#include "read.h"
#include "matrix.h"

int mm_read(char* filename, matrix* mat) 
{
/*
Notes:
  Use fopen to open the matrix file, then use fgets or getline to read lines
  The first line of the matrix contains integers for "#rows #columns"
  - Use these to allocate your arrays for matrix data
  Each subsequent line contains a single data value
  - You can use sscanf to parse each matrix entry
  - First value is data[0][0], next is data[0][1] ... data[#rows-1][#columns-1]
*/
  FILE * infile;
  char current_line[25];
  int first_char;
  int rows_num, cols_num, i, j;
  double data_point;
  infile = fopen (filename, "r");
  if(infile == NULL) {
    //if the file does not exist, the program is terminated
    printf("mm_read: failed to open file.\n");
    exit(-1);
  }
  first_char = fgetc(infile);
  if (first_char == EOF) {
    //EOF means end of file
    //if the file is empty (or the first line is), the program is terminated
    printf("mm_read: failed to read from file.\n");
    exit(-1);
  }
  ungetc(first_char, infile);
  fgets(current_line, 25, infile);
  if (strchr(current_line, ' ') == NULL) {
    //if the first line is a single number (aka no spaces)
    printf("mm_read: failed to read matrix dimensions.\n");
    exit(-1);
  }
  rows_num = atoi(strtok(current_line, " "));
  cols_num = atoi(strtok(NULL, " "));
  mat -> rows = rows_num;
  mat -> cols = cols_num;
  mm_alloc(mat);
  //Begins putting values into the matrix as they are read
  int matrix_value_counter = 0;
  for (i = 0; i < rows_num; i++) {
    for (j = 0; j < cols_num; j++) {
      if (feof(infile)) {
        //If the matrix file doesn’t contain enough values:
        printf("mm_read: failed to read matrix values.\n");
        exit(-1);
      }
      fgets(current_line, 25, infile);
      sscanf(current_line, "%lf", &data_point);
      mat -> data[i][j] = data_point;
      matrix_value_counter++;
    }
  }
  fclose(infile);
  
  return 0;
}