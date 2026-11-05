#include <stdlib.h>

#include "s21_matrix.h"
#include "s21_matrix_internal.h"

int s21_eq_matrix(s21_matrix *A, s21_matrix *B) {
  if (!s21_is_valid_matrix(A) || !s21_is_valid_matrix(B)) {
    return FAILURE;
  }

  int exit_code = SUCCESS;

  if (s21_is_same_size(A, B)) {
    for (int i = 0; i < A->rows && exit_code != FAILURE; i++) {
      for (int j = 0; j < A->columns && exit_code != FAILURE; j++) {
        exit_code = s21_double_eq(A->matrix[i][j], B->matrix[i][j]);
      }
    }
  } else
    exit_code = FAILURE;
  return exit_code;
}

// result - должен быть свежий
int s21_sum_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result) {
  if (!s21_is_valid_matrix(A) || !s21_is_valid_matrix(B) || result == NULL) {
    return 1;
  }

  if (s21_is_valid_matrix(result)) {
    s21_remove_matrix(result);
  }

  int exit_code = 0;
  if (s21_is_same_size(A, B)) {
    exit_code = s21_create_matrix(A->rows, A->columns, result);

    for (int i = 0; i < A->rows && exit_code == 0; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
      }
    }
  } else
    exit_code = 2;
  return exit_code;
}

int s21_sub_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result) {
  if (!s21_is_valid_matrix(A) || !s21_is_valid_matrix(B) || result == NULL) {
    return 1;
  }

  if (s21_is_valid_matrix(result)) {
    s21_remove_matrix(result);
  }

  int exit_code = 0;
  if (s21_is_same_size(A, B)) {
    exit_code = s21_create_matrix(A->rows, A->columns, result);

    for (int i = 0; i < A->rows && exit_code == 0; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
      }
    }
  } else
    exit_code = 2;
  return exit_code;
}

int s21_mult_number(s21_matrix *A, double number, s21_matrix *result) {
  if (!s21_is_valid_matrix(A) || result == NULL) {
    return 1;
  }

  if (s21_is_valid_matrix(result)) {
    s21_remove_matrix(result);
  }

  int exit_code = s21_create_matrix(A->rows, A->columns, result);

  for (int i = 0; i < A->rows && exit_code == 0; i++) {
    for (int j = 0; j < A->columns; j++) {
      result->matrix[i][j] = A->matrix[i][j] * number;
    }
  }

  return exit_code;
}

int s21_mult_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result) {
  if (!s21_is_valid_matrix(A) || !s21_is_valid_matrix(B) || result == NULL) {
    return 1;
  }

  if (s21_is_valid_matrix(result)) {
    s21_remove_matrix(result);
  }

  int exit_code = 0;
  if (A->columns != B->rows) {
    exit_code = 2;
  } else {
    exit_code = s21_create_matrix(A->rows, B->columns, result);
  }

  for (int i = 0; i < A->rows && exit_code == 0; i++) {
    for (int j = 0; j < B->columns; j++) {
      double sum = 0;
      for (int k = 0; k < A->columns; k++) {
        sum += A->matrix[i][k] * B->matrix[k][j];
      }
      result->matrix[i][j] = sum;
    }
  }

  return exit_code;
}
