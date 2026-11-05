#ifndef S21_MATRIX_H
#define S21_MATRIX_H

#define SUCCESS 1
#define FAILURE 0

typedef struct matrix_struct {
  double **matrix;
  int rows;
  int columns;
} s21_matrix;

int s21_create_matrix(int rows, int columns, s21_matrix *result);
void s21_remove_matrix(s21_matrix *A);

int s21_eq_matrix(s21_matrix *A, s21_matrix *B);
int s21_sum_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result);
int s21_sub_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result);
int s21_mult_number(s21_matrix *A, double number, s21_matrix *result);
int s21_mult_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result);

int s21_transpose(s21_matrix *A, s21_matrix *result);
int s21_calc_complements(s21_matrix *A, s21_matrix *result);
int s21_determinant(s21_matrix *A, double *result);
int s21_inverse_matrix(s21_matrix *A, s21_matrix *result);

#endif