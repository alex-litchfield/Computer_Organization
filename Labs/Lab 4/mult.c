#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#include "mult.h"
#include "matrix.h"

matrix* mm_matrix_mult(matrix* mat1, matrix* mat2)
{
/*
Notes:
  First check that it is actually possible to multiply the two inputs
  If so, construct a result matrix
  Then perform the multiplication
  Then return a pointer to your result matrix
*/
  if (mat1->cols != mat2->rows) {
    //if the size of the rows of matrix1 is NOT the same as the size of the columns of matrix2
    printf("mm_matrix_mult: dimension mismatch between matrices.\n");
    exit(-1);
  }

  //matrix construction
  matrix* result_matrix = malloc(sizeof(matrix));
  result_matrix -> rows = mat1->rows;
  result_matrix -> cols = mat2->cols;
  mm_alloc(result_matrix);

  //matrix multiplication
  // Multiplying first and second matrices and storing it in result
  int i, j, k;
  for (i = 0; i < mat1->rows; i++) {
    for (j = 0; j < mat2->cols; j++) {
      for (k = 0; k < mat1->cols; k++) {
        result_matrix->data[i][j] = mat1->data[i][k] * mat2->data[k][j] + result_matrix->data[i][j];
      }
    }
  }

  return result_matrix;
}