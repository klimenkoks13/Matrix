#include <math.h>
#include <stdlib.h>

#include "s21_matrix.h"
#include "s21_matrix_internal.h"

int s21_transpose(s21_matrix *A, s21_matrix *result) {
  if (!s21_is_valid_matrix(A) || result == NULL) {
    return 1;
  }

  if (s21_is_valid_matrix(result)) {
    s21_remove_matrix(result);
  }

  int exit_code = s21_create_matrix(A->columns, A->rows, result);

  for (int i = 0; i < A->rows && exit_code == 0; i++) {
    for (int j = 0; j < A->columns; j++) {
      result->matrix[j][i] = A->matrix[i][j];
    }
  }

  return exit_code;
}

int s21_calc_complements(s21_matrix *A, s21_matrix *result) {
  if (!s21_is_valid_matrix(A) || result == NULL) {
    return 1;
  }
  int exit_code = 0;

  if (!s21_is_square(A)) {
    exit_code = 2;
  } else {
    if (s21_is_valid_matrix(result)) {
      s21_remove_matrix(result);
    }
    exit_code = s21_create_matrix(A->rows, A->columns, result);
  }

  for (int i = 0; i < A->rows && exit_code == 0; i++) {
    for (int j = 0; j < A->columns; j++) {
      result->matrix[i][j] = s21_calc_complement(A, i, j);
    }
  }

  return exit_code;
}

int s21_determinant(s21_matrix *A, double *result) {
  if (!s21_is_valid_matrix(A) || result == NULL) {
    return 1;
  }
  int exit_code = 0;

  if (!s21_is_square(A)) {
    exit_code = 2;
  } else {
    *result = s21_determinant_recursive(A);
    if (isnan(*result)) {
      exit_code = 2;
    }
  }

  return exit_code;
}

int s21_inverse_matrix(s21_matrix *A, s21_matrix *result) {
  if (!s21_is_valid_matrix(A) || result == NULL) {
    return 1;
  }

  int exit_code = 0;
  double det = 0.0;

  if (!s21_is_square(A)) {
    exit_code = 2;
  } else {
    s21_determinant(A, &det);
    if (s21_double_eq(det, 0.0)) {
      exit_code = 2;
    }
  }

  if (exit_code == 0) {
    s21_matrix complements = {NULL, 0, 0};
    s21_matrix transposed = {NULL, 0, 0};

    exit_code = s21_calc_complements(A, &complements);

    if (exit_code == 0) {
      exit_code = s21_transpose(&complements, &transposed);
    }
    s21_remove_matrix(&complements);

    if (exit_code == 0) {
      exit_code = s21_mult_number(&transposed, 1.0 / det, result);
    }
    s21_remove_matrix(&transposed);
  }
  return exit_code;
}
