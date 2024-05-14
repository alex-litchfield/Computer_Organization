#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#include "alloc.h"
#include "matrix.h"

int mm_alloc(matrix* mat)
{
/*
Notes:
  Allocate an array of pointers to a double
  Then, allocate an row/array of doubles and assign each double pointer that row
  Make sure you're allocating using mat->data in the matrix struct
  You can use calloc for allocation and zeroing out all the bytes
  - otherwise, you'd want to manually set all arrays to 0 (good practice)
  If your malloc/calloc returns null, the allocation failed
*/
  if (mat->rows <= 0 || mat->cols <=0) {
    printf("mm_alloc: allocation failed.\n");
    exit(-1);
  }
  mat->data = calloc(mat->rows, sizeof(double*));

  int i;
  for (i = 0; i < mat->rows; i++) {
    *(mat->data + i) = calloc(mat->cols, sizeof(double));
  }
  return 0;
}