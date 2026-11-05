#include "s21_matrix.h"

#include <check.h>
#include <math.h>
#include <sys/resource.h>

#include "s21_matrix_internal.h"

#define MATRIX_ZERO {NULL, 0, 0}

/* ─── Хелперы ───────────────────────────────────────────────── */

static void fill(s21_matrix *A, const double *vals) {
  int k = 0;
  for (int i = 0; i < A->rows; i++)
    for (int j = 0; j < A->columns; j++) A->matrix[i][j] = vals[k++];
}

static void assert_matrix(const s21_matrix *A, const double *expected) {
  int k = 0;
  for (int i = 0; i < A->rows; i++)
    for (int j = 0; j < A->columns; j++)
      ck_assert_double_eq(A->matrix[i][j], expected[k++]);
}

/* БАЗОВЫЕ ФУНКЦИИ */

/* s21_create_matrix */

START_TEST(test_create_ok) {
  s21_matrix A = MATRIX_ZERO;
  int code = s21_create_matrix(2, 3, &A);
  ck_assert_int_eq(code, 0);
  ck_assert_int_eq(A.rows, 2);
  ck_assert_int_eq(A.columns, 3);
  ck_assert_ptr_nonnull(A.matrix);
  ck_assert_ptr_nonnull(A.matrix[0]);
  ck_assert_ptr_nonnull(A.matrix[1]);
  ck_assert_double_eq(A.matrix[0][0], 0.0);
  ck_assert_double_eq(A.matrix[1][2], 0.0);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_create_null_result) {
  ck_assert_int_eq(s21_create_matrix(2, 2, NULL), 1);
}
END_TEST

START_TEST(test_create_bad_rows) {
  s21_matrix A = MATRIX_ZERO;
  ck_assert_int_eq(s21_create_matrix(0, 2, &A), 1);
  ck_assert_int_eq(s21_create_matrix(-1, 2, &A), 1);
}
END_TEST

START_TEST(test_create_bad_cols) {
  s21_matrix A = MATRIX_ZERO;
  ck_assert_int_eq(s21_create_matrix(2, 0, &A), 1);
  ck_assert_int_eq(s21_create_matrix(2, -3, &A), 1);
}
END_TEST

START_TEST(test_create_1x1) {
  s21_matrix A = MATRIX_ZERO;
  ck_assert_int_eq(s21_create_matrix(1, 1, &A), 0);
  ck_assert_int_eq(A.rows, 1);
  ck_assert_int_eq(A.columns, 1);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_create_small_sizes) {
  s21_matrix A = MATRIX_ZERO;
  ck_assert_int_eq(s21_create_matrix(1, 5, &A), 0);
  ck_assert_int_eq(A.rows, 1);
  ck_assert_int_eq(A.columns, 5);
  s21_remove_matrix(&A);

  ck_assert_int_eq(s21_create_matrix(5, 1, &A), 0);
  ck_assert_int_eq(A.rows, 5);
  ck_assert_int_eq(A.columns, 1);
  s21_remove_matrix(&A);

  ck_assert_int_eq(s21_create_matrix(10, 10, &A), 0);
  ck_assert_int_eq(A.rows, 10);
  ck_assert_int_eq(A.columns, 10);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_create_alloc_array_fails) {
  struct rlimit old_rl, new_rl;
  getrlimit(RLIMIT_AS, &old_rl);
  new_rl.rlim_cur = 64 * 1024 * 1024;
  new_rl.rlim_max = old_rl.rlim_max;
  setrlimit(RLIMIT_AS, &new_rl);

  s21_matrix A = MATRIX_ZERO;
  ck_assert_int_eq(s21_create_matrix(10000000, 1, &A), 1);
  ck_assert_ptr_null(A.matrix);
  ck_assert_int_eq(A.rows, 0);
  ck_assert_int_eq(A.columns, 0);

  setrlimit(RLIMIT_AS, &old_rl);
}
END_TEST

START_TEST(test_create_alloc_row_fails) {
  struct rlimit old_rl, new_rl;
  getrlimit(RLIMIT_AS, &old_rl);
  new_rl.rlim_cur = 64 * 1024 * 1024;
  new_rl.rlim_max = old_rl.rlim_max;
  setrlimit(RLIMIT_AS, &new_rl);

  s21_matrix A = MATRIX_ZERO;
  ck_assert_int_eq(s21_create_matrix(10, 10000000, &A), 1);
  ck_assert_ptr_null(A.matrix);
  ck_assert_int_eq(A.rows, 0);
  ck_assert_int_eq(A.columns, 0);

  setrlimit(RLIMIT_AS, &old_rl);
}
END_TEST

/* s21_remove_matrix */

START_TEST(test_remove_null) {
  s21_remove_matrix(NULL);  // не должно упасть
}
END_TEST

START_TEST(test_remove_twice) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_remove_matrix(&A);
  ck_assert_ptr_null(A.matrix);
  ck_assert_int_eq(A.rows, 0);
  ck_assert_int_eq(A.columns, 0);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_set_zero */

START_TEST(test_set_zero) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_set_zero(&A);
  ck_assert_ptr_null(A.matrix);
  ck_assert_int_eq(A.rows, 0);
  ck_assert_int_eq(A.columns, 0);
}
END_TEST

/*  s21_is_valid_matrix */

START_TEST(test_is_valid_ok) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_is_valid_matrix(&A), 1);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_is_valid_null) {
  ck_assert_int_eq(s21_is_valid_matrix(NULL), 0);
}
END_TEST

START_TEST(test_is_valid_no_matrix) {
  s21_matrix A = {NULL, 2, 2};
  ck_assert_int_eq(s21_is_valid_matrix(&A), 0);
}
END_TEST

START_TEST(test_is_valid_bad_rows) {
  s21_matrix A = {NULL, 0, 2};
  ck_assert_int_eq(s21_is_valid_matrix(&A), 0);
}
END_TEST

START_TEST(test_is_valid_bad_cols) {
  s21_matrix A = {NULL, 2, 0};
  ck_assert_int_eq(s21_is_valid_matrix(&A), 0);
}
END_TEST

/* s21_is_square */

START_TEST(test_is_square_true) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);
  ck_assert_int_eq(s21_is_square(&A), 1);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_is_square_false) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  ck_assert_int_eq(s21_is_square(&A), 0);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_is_same_size */

START_TEST(test_is_same_size_true) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  s21_create_matrix(2, 3, &B);
  ck_assert_int_eq(s21_is_same_size(&A, &B), 1);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_is_same_size_false) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  s21_create_matrix(3, 2, &B);
  ck_assert_int_eq(s21_is_same_size(&A, &B), 0);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

/* s21_double_eq */

START_TEST(test_double_eq_true) {
  ck_assert_int_eq(s21_double_eq(1.0, 1.0), 1);
  ck_assert_int_eq(s21_double_eq(0.0, 0.0), 1);
  ck_assert_int_eq(s21_double_eq(0.1 + 0.2, 0.3), 1);
}
END_TEST

START_TEST(test_double_eq_false) {
  ck_assert_int_eq(s21_double_eq(1.0, 1.1), 0);
  ck_assert_int_eq(s21_double_eq(-1.0, 1.0), 0);
}
END_TEST

START_TEST(test_double_eq_epsilon) {
  ck_assert_int_eq(s21_double_eq(1.0, 1.0 + 5e-7), 1);
  ck_assert_int_eq(s21_double_eq(1.0, 1.0 + 2e-6), 0);
}
END_TEST

/* s21_eq_matrix */

START_TEST(test_eq_equal) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);
  double v[] = {1, 2, 3, 4};
  fill(&A, v);
  fill(&B, v);
  ck_assert_int_eq(s21_eq_matrix(&A, &B), SUCCESS);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_not_equal) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);
  double v1[] = {1, 2, 3, 4};
  double v2[] = {1, 2, 3, 5};
  fill(&A, v1);
  fill(&B, v2);
  ck_assert_int_eq(s21_eq_matrix(&A, &B), FAILURE);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_first_diff_last_equal) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);
  double v1[] = {9, 2, 3, 4};
  double v2[] = {1, 2, 3, 4};
  fill(&A, v1);
  fill(&B, v2);
  ck_assert_int_eq(s21_eq_matrix(&A, &B), FAILURE);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_diff_size) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 3, &B);
  ck_assert_int_eq(s21_eq_matrix(&A, &B), FAILURE);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_invalid) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_eq_matrix(&A, NULL), FAILURE);
  ck_assert_int_eq(s21_eq_matrix(NULL, &A), FAILURE);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_copy_matrix */

START_TEST(test_copy_ok) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  double v[] = {1, 2, 3, 4, 5, 6};
  fill(&A, v);

  ck_assert_int_eq(s21_copy_matrix(&A, &B), 0);
  ck_assert_int_eq(s21_eq_matrix(&A, &B), SUCCESS);

  B.matrix[0][0] = 99;
  ck_assert_double_eq(A.matrix[0][0], 1.0);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_copy_invalid_src) {
  s21_matrix B = MATRIX_ZERO;
  ck_assert_int_eq(s21_copy_matrix(NULL, &B), 1);
}
END_TEST

START_TEST(test_copy_null_dst) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_copy_matrix(&A, NULL), 1);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_get_minor_ok) {
  s21_matrix A = MATRIX_ZERO, minor = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);
  double va[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  fill(&A, va);

  ck_assert_int_eq(s21_get_minor(&A, 1, 1, &minor), 0);
  ck_assert_int_eq(minor.rows, 2);
  ck_assert_int_eq(minor.columns, 2);
  double exp[] = {1, 3, 7, 9};
  assert_matrix(&minor, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&minor);
}
END_TEST

START_TEST(test_get_minor_invalid_args) {
  s21_matrix A = MATRIX_ZERO, minor = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);

  ck_assert_int_eq(s21_get_minor(&A, -1, 0, &minor), 1);
  ck_assert_int_eq(s21_get_minor(&A, 3, 0, &minor), 1);

  ck_assert_int_eq(s21_get_minor(&A, 0, -1, &minor), 1);
  ck_assert_int_eq(s21_get_minor(&A, 0, 3, &minor), 1);

  ck_assert_int_eq(s21_get_minor(&A, 0, 0, NULL), 1);

  ck_assert_int_eq(s21_get_minor(NULL, 0, 0, &minor), 1);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_get_minor_1x1_fails) {
  s21_matrix A = MATRIX_ZERO, minor = MATRIX_ZERO;
  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;

  ck_assert_int_eq(s21_get_minor(&A, 0, 0, &minor), 1);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_double_eq_special) {
  ck_assert_int_eq(s21_double_eq(NAN, NAN), 0);
  ck_assert_int_eq(s21_double_eq(NAN, 1.0), 0);
  ck_assert_int_eq(s21_double_eq(INFINITY, INFINITY), 0);
  ck_assert_int_eq(s21_double_eq(-INFINITY, INFINITY), 0);
}
END_TEST

/* ФУНКЦИИ АРИФМЕТИКИ */

/* s21_sum_matrix */

START_TEST(test_sum_ok) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);
  double va[] = {1, 2, 3, 4};
  double vb[] = {5, 6, 7, 8};
  fill(&A, va);
  fill(&B, vb);

  ck_assert_int_eq(s21_sum_matrix(&A, &B, &R), 0);
  double exp[] = {6, 8, 10, 12};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_sum_diff_size) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 3, &B);
  ck_assert_int_eq(s21_sum_matrix(&A, &B, &R), 2);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_invalid) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_sum_matrix(&A, NULL, &R), 1);
  ck_assert_int_eq(s21_sum_matrix(NULL, &A, &R), 1);
  ck_assert_int_eq(s21_sum_matrix(&A, &A, NULL), 1);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_sub_matrix */

START_TEST(test_sub_ok) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);
  double va[] = {5, 6, 7, 8};
  double vb[] = {1, 2, 3, 4};
  fill(&A, va);
  fill(&B, vb);

  ck_assert_int_eq(s21_sub_matrix(&A, &B, &R), 0);
  double exp[] = {4, 4, 4, 4};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_sub_diff_size) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(3, 2, &A);
  s21_create_matrix(2, 3, &B);
  ck_assert_int_eq(s21_sub_matrix(&A, &B, &R), 2);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_invalid) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_sub_matrix(NULL, &A, &R), 1);
  ck_assert_int_eq(s21_sub_matrix(&A, NULL, &R), 1);
  ck_assert_int_eq(s21_sub_matrix(&A, &A, NULL), 1);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_mult_number */

START_TEST(test_mult_number_ok) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  double va[] = {1, 2, 3, 4, 5, 6};
  fill(&A, va);

  ck_assert_int_eq(s21_mult_number(&A, 2.0, &R), 0);
  double exp[] = {2, 4, 6, 8, 10, 12};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_mult_number_zero) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);

  ck_assert_int_eq(s21_mult_number(&A, 0.0, &R), 0);
  double exp[] = {0, 0, 0, 0};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_mult_number_negative) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, -2, 3, -4};
  fill(&A, va);

  ck_assert_int_eq(s21_mult_number(&A, -1.0, &R), 0);
  double exp[] = {-1, 2, -3, 4};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_mult_number_invalid) {
  s21_matrix R = MATRIX_ZERO;
  ck_assert_int_eq(s21_mult_number(NULL, 2.0, &R), 1);
}
END_TEST

START_TEST(test_mult_number_null_result) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_mult_number(&A, 2.0, NULL), 1);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_mult_matrix */

START_TEST(test_mult_matrix_square) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);
  double va[] = {1, 2, 3, 4};
  double vb[] = {5, 6, 7, 8};
  fill(&A, va);
  fill(&B, vb);

  ck_assert_int_eq(s21_mult_matrix(&A, &B, &R), 0);
  double exp[] = {19, 22, 43, 50};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_mult_matrix_rect) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  s21_create_matrix(3, 2, &B);
  double va[] = {1, 2, 3, 4, 5, 6};
  double vb[] = {7, 8, 9, 10, 11, 12};
  fill(&A, va);
  fill(&B, vb);

  ck_assert_int_eq(s21_mult_matrix(&A, &B, &R), 0);
  double exp[] = {58, 64, 139, 154};
  assert_matrix(&R, exp);
  ck_assert_int_eq(R.rows, 2);
  ck_assert_int_eq(R.columns, 2);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_mult_matrix_identity) {
  s21_matrix A = MATRIX_ZERO, I = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);
  s21_create_matrix(3, 3, &I);
  double va[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  double vi[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  fill(&A, va);
  fill(&I, vi);

  ck_assert_int_eq(s21_mult_matrix(&A, &I, &R), 0);
  assert_matrix(&R, va);

  s21_remove_matrix(&A);
  s21_remove_matrix(&I);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_mult_matrix_zero) {
  s21_matrix A = MATRIX_ZERO, Z = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &Z);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);

  ck_assert_int_eq(s21_mult_matrix(&A, &Z, &R), 0);
  double exp[] = {0, 0, 0, 0};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&Z);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_mult_matrix_mismatch) {
  s21_matrix A = MATRIX_ZERO, B = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  s21_create_matrix(2, 3, &B);
  ck_assert_int_eq(s21_mult_matrix(&A, &B, &R), 2);
  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_invalid) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_mult_matrix(NULL, &A, &R), 1);
  ck_assert_int_eq(s21_mult_matrix(&A, NULL, &R), 1);
  ck_assert_int_eq(s21_mult_matrix(&A, &A, NULL), 1);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_transpose */

START_TEST(test_transpose_ok) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  double va[] = {1, 2, 3, 4, 5, 6};
  fill(&A, va);

  ck_assert_int_eq(s21_transpose(&A, &R), 0);
  ck_assert_int_eq(R.rows, 3);
  ck_assert_int_eq(R.columns, 2);
  double exp[] = {1, 4, 2, 5, 3, 6};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_transpose_square) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);

  ck_assert_int_eq(s21_transpose(&A, &R), 0);
  double exp[] = {1, 3, 2, 4};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_transpose_1xN) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(1, 3, &A);
  double va[] = {1, 2, 3};
  fill(&A, va);

  ck_assert_int_eq(s21_transpose(&A, &R), 0);
  ck_assert_int_eq(R.rows, 3);
  ck_assert_int_eq(R.columns, 1);
  double exp[] = {1, 2, 3};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_transpose_invalid) {
  s21_matrix R = MATRIX_ZERO;
  ck_assert_int_eq(s21_transpose(NULL, &R), 1);

  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  ck_assert_int_eq(s21_transpose(&A, NULL), 1);
  s21_remove_matrix(&A);
}
END_TEST

/* s21_calc_complements */

START_TEST(test_calc_complements_3x3) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);
  double va[] = {1, 2, 3, 0, 4, 2, 5, 2, 1};
  fill(&A, va);

  ck_assert_int_eq(s21_calc_complements(&A, &R), 0);
  ck_assert_int_eq(R.rows, 3);
  ck_assert_int_eq(R.columns, 3);
  double exp[] = {0, 10, -20, 4, -14, 8, -8, -2, 4};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_calc_complements_2x2) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);
  ck_assert_int_eq(s21_calc_complements(&A, &R), 0);
  double exp[] = {4, -3, -2, 1};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_calc_complements_1x1) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;

  ck_assert_int_eq(s21_calc_complements(&A, &R), 0);
  ck_assert_double_eq(R.matrix[0][0], 1.0);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_calc_complements_not_square) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);

  ck_assert_int_eq(s21_calc_complements(&A, &R), 2);
  ck_assert_ptr_null(R.matrix);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_calc_complements_invalid) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);

  ck_assert_int_eq(s21_calc_complements(NULL, &R), 1);
  ck_assert_int_eq(s21_calc_complements(&A, NULL), 1);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_calc_complements_into_existing_result) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(5, 5, &R);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);

  ck_assert_int_eq(s21_calc_complements(&A, &R), 0);
  ck_assert_int_eq(R.rows, 2);
  ck_assert_int_eq(R.columns, 2);
  double exp[] = {4, -3, -2, 1};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

/* s21_determinant */

START_TEST(test_determinant_1x1) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;
  double result = 0.0;

  ck_assert_int_eq(s21_determinant(&A, &result), 0);
  ck_assert_double_eq(result, 5.0);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_2x2) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);
  double result = 0.0;

  ck_assert_int_eq(s21_determinant(&A, &result), 0);
  ck_assert_double_eq(result, -2.0);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_3x3) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);
  double va[] = {1, 2, 3, 0, 4, 2, 5, 2, 1};
  fill(&A, va);
  double result = 0.0;

  ck_assert_int_eq(s21_determinant(&A, &result), 0);
  ck_assert_double_eq(result, -40.0);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_4x4) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(4, 4, &A);
  double va[] = {1, 2, 3, 4, 0, 2, 3, 4, 0, 0, 3, 4, 0, 0, 0, 4};
  fill(&A, va);
  double result = 0.0;

  ck_assert_int_eq(s21_determinant(&A, &result), 0);
  ck_assert_double_eq(result, 24.0);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_zero) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, 2, 2, 4};
  fill(&A, va);
  double result = 123.0;

  ck_assert_int_eq(s21_determinant(&A, &result), 0);
  ck_assert_double_eq(result, 0.0);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_not_square) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);
  double result = 0.0;

  ck_assert_int_eq(s21_determinant(&A, &result), 2);
  ck_assert_double_eq(result, 0.0);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_invalid) {
  s21_matrix A = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double result = 0.0;

  ck_assert_int_eq(s21_determinant(NULL, &result), 1);
  ck_assert_int_eq(s21_determinant(&A, NULL), 1);

  s21_remove_matrix(&A);
}
END_TEST

/* s21_inverse_matrix */

START_TEST(test_inverse_1x1) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 4.0;
  double exp[] = {0.25};

  ck_assert_int_eq(s21_inverse_matrix(&A, &R), 0);
  ck_assert_int_eq(R.rows, 1);
  ck_assert_int_eq(R.columns, 1);
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_inverse_2x2) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);

  ck_assert_int_eq(s21_inverse_matrix(&A, &R), 0);
  double exp[] = {-2.0, 1.0, 1.5, -0.5};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_inverse_3x3) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);
  double va[] = {2, 5, 7, 6, 3, 4, 5, -2, -3};
  fill(&A, va);

  ck_assert_int_eq(s21_inverse_matrix(&A, &R), 0);
  double exp[] = {1.0, -1.0, 1.0, -38.0, 41.0, -34.0, 27.0, -29.0, 24.0};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST
START_TEST(test_inverse_identity) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(3, 3, &A);
  double va[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  fill(&A, va);

  ck_assert_int_eq(s21_inverse_matrix(&A, &R), 0);
  assert_matrix(&R, va);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

START_TEST(test_inverse_singular) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  double va[] = {1, 2, 2, 4};
  fill(&A, va);

  ck_assert_int_eq(s21_inverse_matrix(&A, &R), 2);
  ck_assert_ptr_null(R.matrix);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_not_square) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 3, &A);

  ck_assert_int_eq(s21_inverse_matrix(&A, &R), 2);
  ck_assert_ptr_null(R.matrix);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_invalid) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);

  ck_assert_int_eq(s21_inverse_matrix(NULL, &R), 1);
  ck_assert_int_eq(s21_inverse_matrix(&A, NULL), 1);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_into_existing_result) {
  s21_matrix A = MATRIX_ZERO, R = MATRIX_ZERO;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(4, 4, &R);
  double va[] = {1, 2, 3, 4};
  fill(&A, va);

  ck_assert_int_eq(s21_inverse_matrix(&A, &R), 0);
  ck_assert_int_eq(R.rows, 2);
  ck_assert_int_eq(R.columns, 2);
  double exp[] = {-2.0, 1.0, 1.5, -0.5};
  assert_matrix(&R, exp);

  s21_remove_matrix(&A);
  s21_remove_matrix(&R);
}
END_TEST

/* СЮИТЫ */

static Suite *basic_suite(void) {
  Suite *s = suite_create("basic");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_create_ok);
  tcase_add_test(tc, test_create_null_result);
  tcase_add_test(tc, test_create_bad_rows);
  tcase_add_test(tc, test_create_bad_cols);
  tcase_add_test(tc, test_create_1x1);
  tcase_add_test(tc, test_create_small_sizes);
  tcase_add_test(tc, test_create_alloc_array_fails);
  tcase_add_test(tc, test_create_alloc_row_fails);
  tcase_add_test(tc, test_remove_null);
  tcase_add_test(tc, test_remove_twice);
  tcase_add_test(tc, test_set_zero);
  tcase_add_test(tc, test_is_valid_ok);
  tcase_add_test(tc, test_is_valid_null);
  tcase_add_test(tc, test_is_valid_no_matrix);
  tcase_add_test(tc, test_is_valid_bad_rows);
  tcase_add_test(tc, test_is_valid_bad_cols);
  tcase_add_test(tc, test_is_square_true);
  tcase_add_test(tc, test_is_square_false);
  tcase_add_test(tc, test_is_same_size_true);
  tcase_add_test(tc, test_is_same_size_false);
  tcase_add_test(tc, test_double_eq_true);
  tcase_add_test(tc, test_double_eq_false);
  tcase_add_test(tc, test_double_eq_epsilon);
  tcase_add_test(tc, test_eq_equal);
  tcase_add_test(tc, test_eq_not_equal);
  tcase_add_test(tc, test_eq_first_diff_last_equal);
  tcase_add_test(tc, test_eq_diff_size);
  tcase_add_test(tc, test_eq_invalid);
  tcase_add_test(tc, test_copy_ok);
  tcase_add_test(tc, test_copy_invalid_src);
  tcase_add_test(tc, test_copy_null_dst);
  tcase_add_test(tc, test_get_minor_invalid_args);
  tcase_add_test(tc, test_get_minor_ok);
  tcase_add_test(tc, test_get_minor_1x1_fails);
  tcase_add_test(tc, test_double_eq_special);

  suite_add_tcase(s, tc);
  return s;
}

static Suite *arithmetic_suite(void) {
  Suite *s = suite_create("arithmetic");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_sum_ok);
  tcase_add_test(tc, test_sum_diff_size);
  tcase_add_test(tc, test_sum_invalid);
  tcase_add_test(tc, test_sub_ok);
  tcase_add_test(tc, test_sub_diff_size);
  tcase_add_test(tc, test_sub_invalid);
  tcase_add_test(tc, test_mult_number_ok);
  tcase_add_test(tc, test_mult_number_zero);
  tcase_add_test(tc, test_mult_number_negative);
  tcase_add_test(tc, test_mult_number_invalid);
  tcase_add_test(tc, test_mult_number_null_result);
  tcase_add_test(tc, test_mult_matrix_square);
  tcase_add_test(tc, test_mult_matrix_rect);
  tcase_add_test(tc, test_mult_matrix_identity);
  tcase_add_test(tc, test_mult_matrix_zero);
  tcase_add_test(tc, test_mult_matrix_mismatch);
  tcase_add_test(tc, test_mult_matrix_invalid);

  suite_add_tcase(s, tc);
  return s;
}

static Suite *advanced_suite(void) {
  Suite *s = suite_create("advanced");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_transpose_ok);
  tcase_add_test(tc, test_transpose_square);
  tcase_add_test(tc, test_transpose_1xN);
  tcase_add_test(tc, test_transpose_invalid);

  tcase_add_test(tc, test_calc_complements_3x3);
  tcase_add_test(tc, test_calc_complements_2x2);
  tcase_add_test(tc, test_calc_complements_1x1);
  tcase_add_test(tc, test_calc_complements_not_square);
  tcase_add_test(tc, test_calc_complements_invalid);
  tcase_add_test(tc, test_calc_complements_into_existing_result);

  tcase_add_test(tc, test_determinant_1x1);
  tcase_add_test(tc, test_determinant_2x2);
  tcase_add_test(tc, test_determinant_3x3);
  tcase_add_test(tc, test_determinant_4x4);
  tcase_add_test(tc, test_determinant_zero);
  tcase_add_test(tc, test_determinant_not_square);
  tcase_add_test(tc, test_determinant_invalid);

  tcase_add_test(tc, test_inverse_1x1);
  tcase_add_test(tc, test_inverse_2x2);
  tcase_add_test(tc, test_inverse_3x3);
  tcase_add_test(tc, test_inverse_identity);
  tcase_add_test(tc, test_inverse_singular);
  tcase_add_test(tc, test_inverse_not_square);
  tcase_add_test(tc, test_inverse_invalid);
  tcase_add_test(tc, test_inverse_into_existing_result);

  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  int failed = 0;
  SRunner *sr = srunner_create(basic_suite());
  srunner_add_suite(sr, arithmetic_suite());
  srunner_add_suite(sr, advanced_suite());

  srunner_run_all(sr, CK_VERBOSE);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (failed == 0) ? 0 : 1;
}