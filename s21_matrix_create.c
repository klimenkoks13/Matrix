#include <stdlib.h>

#include "s21_matrix.h"
#include "s21_matrix_internal.h"

int s21_create_matrix(int rows, int columns, s21_matrix *result) {
  if (result == NULL || rows <= 0 || columns <= 0) {
    return 1;
  }

  int exit_code = 0;
  s21_set_zero(result);

  double **matrix = calloc(rows, sizeof(double *));
  if (matrix == NULL) {
    exit_code = 1;
  } else {
    for (int i = 0; i < rows && exit_code == 0; i++) {
      matrix[i] = calloc(columns, sizeof(double));
      if (matrix[i] == NULL) {
        exit_code = 1;
      }
    }
  }

  if (exit_code != 0) {
    if (matrix != NULL) {
      for (int j = 0; j < rows; j++) {
        free(matrix[j]);
      }
      free(matrix);
    }
  } else {
    result->matrix = matrix;
    result->rows = rows;
    result->columns = columns;
  }

  return exit_code;
}

void s21_remove_matrix(s21_matrix *A) {
  if (A != NULL && A->matrix != NULL) {
    for (int j = 0; j < A->rows; j++) {
      free(A->matrix[j]);
    }
    free(A->matrix);
    s21_set_zero(A);
  }
}