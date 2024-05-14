#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#include "print.h"
#include "matrix.h"

int mm_print(matrix* mat)
{
/*
Notes:
  Print all the data values in your matrix
  Go row by row
  At the end of each row, print a newline
  Between each data value, insert a tab
  Use 2 digits of precision after the decimal (use "%10.2lf\t" as format string)
  See output.txt for expected output formatting
*/
  printf("\n/******** START of 2-D Matrix **********************************/\n");
  int i, j;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->cols; j++) {
      printf("%10.2lf\t", mat->data[i][j]);
      if (j == mat->cols - 1) {
         printf("\n");
      }
    }
  }
  printf("/******** END of 2-D Matrix ************************************/\n");
  
  return 0;
}