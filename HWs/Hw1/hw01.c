/* 
Assignment 1: NxM Matrix Multiplication 
CSCI-2500 Fall 2021
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

/******************************************************************************/
/* Function Calls */
/******************************************************************************/

typedef struct{
  double** data;
  int rows;
  int cols;
} matrix; 

int mm_alloc(matrix* mat);
int mm_free (matrix* mat);
int mm_print(matrix* mat);
int mm_read (char* filename, matrix* mat);
matrix* mm_matrix_mult(matrix* mat1, matrix* mat2);


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

int mm_free(matrix* mat)
{
/*
Notes:
  Reverse the allocation process, but you'll be calling free() instead
*/
int i;
  for (i = 0; i < mat->rows; i++) {
    free(*(mat->data + i));
  }
  free(mat->data);
  
  return 0;
}

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

int main()
{
  /*
   * you must keep this function AS IS. 
   * you cannot modify it!
   */
  char filename_A[256];
  char filename_B[256];
  matrix* A = (matrix*)malloc(sizeof(matrix));
  matrix* B = (matrix*)malloc(sizeof(matrix));
  matrix* C = NULL;

  printf("Please enter the matrix A data file name: ");
  if (1 != scanf("%s", filename_A)) {
    printf("main: scanf failed to read filename\n");
    exit(-1);
  }
  mm_read(filename_A, A);
  mm_print(A);
  
  printf("Please enter the matrix B data file name: ");
  if (1 != scanf("%s", filename_B)) {
    printf("main: scanf failed to read filename\n");
    exit(-1);
  }
  mm_read(filename_B, B);
  mm_print(B);
  
  C = mm_matrix_mult(A, B);
  mm_print(C);

  mm_free(A);
  mm_free(B);
  mm_free(C);
  free(A);
  free(B);
  free(C);
  
  return 0;
}
