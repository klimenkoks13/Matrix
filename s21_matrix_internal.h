#ifndef S21_MATRIX_INTERNAL_H
#define S21_MATRIX_INTERNAL_H

#include "s21_matrix.h"

int s21_is_valid_matrix(const s21_matrix *A);
int s21_is_square(const s21_matrix *A);
int s21_is_same_size(const s21_matrix *A, const s21_matrix *B);
int s21_double_eq(double a, double b);

int s21_copy_matrix(const s21_matrix *src, s21_matrix *dst);
void s21_set_zero(s21_matrix *A);

int s21_get_minor(const s21_matrix *A, int row, int col, s21_matrix *minor);
double s21_determinant_recursive(const s21_matrix *A);
double s21_calc_complement(const s21_matrix *A, int row, int col);

#endif