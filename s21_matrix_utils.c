#include <math.h>
#include <stdlib.h>

#include "s21_matrix.h"
#include "s21_matrix_internal.h"

int s21_is_valid_matrix(const s21_matrix *A) {
  return A != NULL && A->matrix != NULL && A->rows > 0 && A->columns > 0;
}

int s21_is_square(const s21_matrix *A) { return A->rows == A->columns; }

int s21_is_same_size(const s21_matrix *A, const s21_matrix *B) {
  return A->rows == B->rows && A->columns == B->columns;
}

// если модуль разности не больше 0.000001 — числа равны
int s21_double_eq(double a, double b) { return fabs(a - b) <= 1e-6; }

int s21_copy_matrix(const s21_matrix *src, s21_matrix *dst) {
  if (!s21_is_valid_matrix(src) || dst == NULL) {
    return 1;
  }

  int exit_code = s21_create_matrix(src->rows, src->columns, dst);

  for (int i = 0; i < src->rows && exit_code == 0; i++) {
    for (int j = 0; j < src->columns; j++) {
      dst->matrix[i][j] = src->matrix[i][j];
    }
  }

  return exit_code;
}

void s21_set_zero(s21_matrix *A) {
  A->matrix = NULL;
  A->rows = 0;
  A->columns = 0;
}

int s21_get_minor(const s21_matrix *A, int row, int col, s21_matrix *minor) {
  if (!s21_is_valid_matrix(A) || minor == NULL || row < 0 || row >= A->rows ||
      col < 0 || col >= A->columns) {
    return 1;
  }

  int exit_code = s21_create_matrix(A->rows - 1, A->columns - 1, minor);

  for (int i = 0; i < A->rows && exit_code == 0; i++) {
    for (int j = 0; j < A->columns; j++) {
      if (i != row && j != col) {
        int mi = (i < row) ? i : i - 1;
        int mj = (j < col) ? j : j - 1;
        minor->matrix[mi][mj] = A->matrix[i][j];
      }
    }
  }

  return exit_code;
}

double s21_determinant_recursive(const s21_matrix *A) {
  double det = NAN;
  int is_valid = 1;

  if (A->rows == 1) {
    det = A->matrix[0][0];
  } else if (A->rows == 2) {
    det = A->matrix[0][0] * A->matrix[1][1] - A->matrix[0][1] * A->matrix[1][0];
  } else {  // разложение по первой строке
    det = 0.0;
    int sign = 1;
    for (int j = 0; j < A->columns && is_valid; j++) {
      s21_matrix minor = {NULL, 0, 0};
      if (s21_get_minor(A, 0, j, &minor) == 0) {
        // рекурсивно считается определитель минора и добавляется
        det += sign * A->matrix[0][j] * s21_determinant_recursive(&minor);
        s21_remove_matrix(&minor);
      } else {
        is_valid = 0;
      }
      sign = -sign;
    }
  }

  return is_valid ? det : NAN;
}

double s21_calc_complement(const s21_matrix *A, int row, int col) {
  double result = NAN;
  if (A->rows == 1) {
    result = 1.0;
  } else {
    s21_matrix minor = {NULL, 0, 0};

    if (s21_get_minor(A, row, col, &minor) == 0) {
      // +1 для чётной суммы, -1 для нечётной
      double sign = ((row + col) % 2 == 0) ? 1.0 : -1.0;
      result = sign * s21_determinant_recursive(&minor);
      s21_remove_matrix(&minor);
    }
  }

  return result;
}
