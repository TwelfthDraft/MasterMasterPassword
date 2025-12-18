#include <stdio.h>

#include "tb_constants.h"

#include "tb_crypt_lib.h"
#include "tb_file_lib.h"

#define LONESHA256_IMPLEMENTATION 1
#include "lonesha256/lonesha256.h"

#define TB_WORDS_IMPLEMENTATION 1
#include "tb_words.h"

int log2_arr[1024] = {-1};
int pow2_arr[1024] = {-1};

int tb_new_digest(digest* digest) {
  int result = tb_new_text(digest);
  return result;
}

int tb_digest_to_hex(text* dst, digest* src) {
  return tb_text_to_hex(dst, src, DIGEST_SIZE);
}

int tb_hex_to_digest(digest* dst, text* src) {
  if (tb_strlen(src->text) != DIGEST_SIZE * 2) {
    return !SUCCESS;
  }
  return tb_hex_to_text(dst, src, DIGEST_SIZE);
}

int tb_text_to_digest(digest* dst, text* src, int src_len) {
  return lonesha256(dst->text, src->text, src_len) == 0 ? SUCCESS : (!SUCCESS);
}

int tb_mix_digests(digest* dst, digest* src1, digest* src2) {
  text msg;

  memcpy(msg.text, src1->text, DIGEST_SIZE);
  memcpy(msg.text + DIGEST_SIZE, src2->text, DIGEST_SIZE);

  return tb_text_to_digest(dst, &msg, DIGEST_SIZE * 2);
}

char* tb_get_word(int index) {
  if (index < 0 || index >= 1024) {
    return NULL;
  }

  return tb_words[index];
}

int tb_find_word(char* word) {
  if (word == NULL) {
    return -1;
  }

  int start = 0;
  int end = 1023;

  while (start <= end) {
    int mid = (start + end + 1) / 2;

    char* mid_word = tb_words[mid];

    int comp = strncmp(mid_word, word, 5);

    if (comp == 0) {
      return mid;
    }

    if (comp > 0) {
      end = mid - 1;
    } else {
      start = mid + 1;
    }
  }

  return -1;
}

int _tb_prompt_word(FILE* in, FILE* out, int word_num, text* word, text* words) {
  if (in == NULL || out == NULL || feof(in) || feof(out)) {
    return !SUCCESS;
  }

  tb_new_text(word);

  fprintf(out, "Please enter word %d\n", word_num);

  if (tb_read_line(in, word) != SUCCESS) {
    fprintf(out, "Failed to read line\n");
    return !SUCCESS;
  }

  if (tb_find_word(word->text) == -1) {
    fprintf(out, "Unknown word %s\n", word->text);
    return !SUCCESS;
  }

  if (words != NULL) {
    text minus;
    tb_new_text(&minus);
    strncpy(minus.text, "-", 2);

    if (words->text[0] != 0) {
      if (tb_textcat(words, &minus) != SUCCESS) {
        return !SUCCESS;
      }
    }

    if (tb_textcat(words, word) != SUCCESS) {
      return !SUCCESS;
    }
  }

  return SUCCESS;
}

int tb_prompt_word(int word_num, text* word, text* words) {
  return _tb_prompt_word(stdin, stdout, word_num, word, words);
}

// Finite field maths
int MOD = 1033;

int _tb_ff_mul(int a, int b) {
  if (a < 0 || a > 1023 || b < 0 || b > 1023) {
    return -1;
  }

  int product = a * b;

  int shift = 10;
  int mod = MOD << shift;

  while (shift >= 0) {
    int next = mod ^ product;
    if (next < product) {
      product = next;
    }
    mod >>= 1;
    shift--;
  }

  return product;
}

void init_cache() {
  if (pow2_arr[0] != -1) {
    return;
  }

  int g = 2;
  int prod = 1;

  for (int i = 0; i < 1023; i++) {
    pow2_arr[i] = prod;
    log2_arr[prod] = i;
    prod = _tb_ff_mul(prod, g);
  }
}

int tb_ff_add(int a, int b) {
  if (a < 0 || a > 1023 || b < 0 || b > 1023) {
    return -1;
  }

  return a ^ b;
}

int tb_ff_mul(int a, int b) {
  init_cache();

  if (a == 0 || b == 0) {
    return 0;
  }

  if (a < 0 || a > 1023 || b < 0 || b > 1023) {
    return -1;
  }

  int logA = log2_arr[a];
  int logB = log2_arr[b];
  int logAB = (logA + logB) % 1023;

  return pow2_arr[logAB];
}

int tb_ff_negate(int a) {
  if (a < 0 || a > 1023) {
    return -1;
  }

  return a;
}

int tb_ff_inverse(int a) {
  init_cache();

  if (a == 0 || a < 0 || a > 1023) {
    return -1;
  }

  int lg2 = log2_arr[a];
  int index = lg2 == 0 ? 0 : (1023 - lg2);

  return pow2_arr[index];
}

int tb_ff_pow2(int n) {
  init_cache();

  if (n < 0 || n >= 1023) {
    n = n % 1023;
    while (n < 0) {
      n += 1023;
    }
  }

  return pow2_arr[n];
}

int tb_ff_log2(int a) {
  init_cache();

  if (a == 0 || a < 0 || a > 1023) {
    return -1;
  }

  return log2_arr[a];
}

void swap_rows(int* a, int size, int* b) {
  if (a != b) {
    for (int col = 0; col < size; col++) {
      int t = a[col];
      a[col] = b[col];
      b[col] = t;
    }
  }
}

void sub_rows(int* a, int size, int* b) {
  for (int col = 0; col < size; col++) {
    a[col] = tb_ff_add(a[col], b[col]);
  }
}

void mul_row(int* row, int size, int a) {
  for (int i = 0; i < size; i++) {
    row[i] = tb_ff_mul(row[i], a);
  }
}

int tb_ff_solve(int x[MATRIX_SIZE], int c[][MATRIX_SIZE], int y[MATRIX_SIZE], int size) {
  if (size > MATRIX_SIZE) {
    return !SUCCESS;
  }

  int c_copy[MATRIX_SIZE][MATRIX_SIZE + 1];

  for (int row = 0; row < size; row++) {
    for (int col = 0; col < size; col++) {
      c_copy[row][col] = c[row][col];
    }
    c_copy[row][size] = y[row];
  }

  for (int col = 0; col < size; col++) {
    // Find a row that starts 0, 0, ... , x, ....
    int ref_row = -1;
    for (int row = 0; row < size; row++) {
      if (c_copy[row][col] != 0) {
        ref_row = row;
        for (int col2 = 0; col2 < col; col2++) {
          if (c_copy[row][col2] != 0) {
            ref_row = -1;
            break;
          }
        }
        if (ref_row != -1) {
          break;
        }
      }
    }

    if (ref_row == -1) {
      return !SUCCESS;
    }

    for (int row = 0; row < size; row++) {
      if (row == ref_row || c_copy[row][col] == 0) {
        continue;
      }

      int ref_val = c_copy[ref_row][col];
      int ref_inv = tb_ff_inverse(ref_val);
      int row_inv = tb_ff_inverse(c_copy[row][col]);

      mul_row(c_copy[row], size + 1, row_inv);
      mul_row(c_copy[row], size + 1, ref_val);
      sub_rows(c_copy[row], size + 1, c_copy[ref_row]);
    }
  }

  for (int col = 0; col < size; col++) {
    for (int row = 0; row < size; row++) {
      if (c_copy[row][col] != 0) {
        int row_inv = tb_ff_inverse(c_copy[row][col]);
        mul_row(c_copy[row], size + 1, row_inv);
        swap_rows(c_copy[row], size + 1, c_copy[col]);
        break;
      }
    }
  }

  for (int row = 0; row < size; row++) {
    x[row] = c_copy[row][size];
  }

  return SUCCESS;
}

int tb_ff_create_polynomial(int coeffs[], int x[], int roots) {
  memset(coeffs + 1, 0, roots * sizeof(*coeffs));
  coeffs[0] = 1;

  for (int i = 0; i < roots; i++) {
    int neg_root = tb_ff_negate(x[i]);

    for (int j = i + 1; j > 0; j--) {
      coeffs[j] = coeffs[j - 1];
    }
    coeffs[0] = 0;

    for (int j = 0; j <= i; j++) {
      coeffs[j] = tb_ff_add(coeffs[j], tb_ff_mul(neg_root, coeffs[j + 1]));
    }
  }

  if (coeffs[0] == 0) {
    return !SUCCESS;
  }

  int c0_inv = tb_ff_inverse(coeffs[0]);
  for (int i = 0; i <= roots; i++) {
    coeffs[i]  = tb_ff_mul(coeffs[i], c0_inv);
  }

  return SUCCESS;
}

int tb_ff_solve_polynomial(int x[], int coeffs[], int roots) {
  int size = roots + 1;

  int pos = 0;

  for (int i = 0; i < 1023; i++) {
    int sum = 0;
    int x_val = i;
    int x_pow = 1;

    for (int j = 0; j < size; j++) {
      sum = tb_ff_add(sum, tb_ff_mul(x_pow, coeffs[j]));
      x_pow = tb_ff_mul(x_pow, x_val);
    }

    if (sum == 0) {
      if (pos >= roots) {
        return !SUCCESS;
      }

      x[pos++] = x_val;
    }
  }

  if (pos != roots) {
    return !SUCCESS;
  }

  return SUCCESS;
}

int tb_ff_evaluate(int y[MATRIX_SIZE], int c[MATRIX_SIZE][MATRIX_SIZE], int x[MATRIX_SIZE], int size) {
  int c_copy[MATRIX_SIZE][MATRIX_SIZE + 1];

  for (int row = 0; row < size; row++) {
    for (int col = 0; col < size; col++) {
      c_copy[row][col] = c[row][col];
    }
  }

  for (int row = 0; row < size; row++) {
    int sum = 0;
    for (int col=0; col < size; col++) {
      sum = tb_ff_add(sum, tb_ff_mul(c[row][col], x[col]));
    }
    y[row] = sum;
  }

  return SUCCESS;
}

int tb_ff_lagrange_general(int coeffs[], int x[], int data[], int size) {
  int c[MATRIX_SIZE][MATRIX_SIZE];

  for (int row = 0; row < size; row++) {
    int x_power = 1;
    for (int col = 0; col < size; col++) {
      c[row][col] = x_power;
      x_power = tb_ff_mul(x_power, x[row]);
    }
  }

  if (tb_ff_solve(coeffs, c, data, size) != SUCCESS) {
    return !SUCCESS;
  }

  return SUCCESS;
}

int tb_ff_inv_lagrange_general(int data[], int x[], int coeffs[], int size) {
  int c[MATRIX_SIZE][MATRIX_SIZE];

  for (int row = 0; row < size; row++) {
    int x_power = 1;
    for (int col = 0; col < size; col++) {
      c[row][col] = x_power;
      x_power = tb_ff_mul(x_power, x[row]);
    }
  }

  for (int row = 0; row < size; row++) {
    int d = 0;
    for (int col = 0; col < size; col++) {
      d = tb_ff_add(d, tb_ff_mul(c[row][col], coeffs[col]));
    }
    data[row] = d;
  }

  return SUCCESS;
}

int tb_ff_lagrange(int coeffs[], int data[], int size) {
  int x[MATRIX_SIZE];

  x[0] = 1;

  for (int row = 1; row < size; row++) {
    x[row] = tb_ff_mul(x[row - 1], 2);
  }

  return tb_ff_lagrange_general(coeffs, x, data, size);
}

int tb_ff_inv_lagrange(int data[], int coeffs[], int size) {
  int c[MATRIX_SIZE][MATRIX_SIZE];

  int x[MATRIX_SIZE];

  x[0] = 1;

  for (int row = 1; row < size; row++) {
    x[row] = tb_ff_mul(x[row - 1], 2);
  }

  return tb_ff_inv_lagrange_general(data, x, coeffs, size);
}

int tb_ff_rs_encode(int coeffs[], int c_size, int data[], int d_size) {
  if (c_size < d_size) {
    return !SUCCESS;
  }

  int ext_data[MATRIX_SIZE];

  int s_size = c_size - d_size;

  memset(ext_data, 0, s_size * sizeof(*ext_data));

  memcpy(ext_data + s_size, data, d_size * sizeof(*data));

  if (tb_ff_lagrange(coeffs, ext_data, c_size) != SUCCESS) {
    return !SUCCESS;
  }

  return SUCCESS;
}

int tb_ff_rs_decode_raw(int data[], int d_size, int syndrome[], int coeffs[], int c_size) {
  if (c_size < d_size) {
    return !SUCCESS;
  }

  int ext_data[MATRIX_SIZE];

  int s_size = c_size - d_size;

  if (tb_ff_inv_lagrange(ext_data, coeffs, c_size) != SUCCESS) {
    return !SUCCESS;
  }

  memcpy(syndrome, ext_data, s_size * sizeof(*syndrome));

  memcpy(data, ext_data + s_size, d_size * sizeof(*data));

  return SUCCESS;
}

int tb_ff_rs_decode_errors(int data[], int d_size, int errors[], int syndrome[], int coeffs[], int c_size) {
  if (c_size < d_size) {
    return !SUCCESS;
  }

  int error_count = 0;
  int error_index[MATRIX_SIZE] = {0};

  for (int i = 0; i < c_size; i++) {
    if (errors[i]) {
      error_index[error_count] = i;
      error_count++;
    }
  }

  int s_size = c_size - d_size;

  if (error_count > s_size) {
    return !SUCCESS;
  }

  int c[MATRIX_SIZE][MATRIX_SIZE];

  for (int col = 0; col < error_count; col++) {
    int x_val = tb_ff_pow2(error_index[col]);
    int x_pow = 1;
    for (int row = 0; row < error_count; row++) {
      c[row][col] = x_pow;
      x_pow = tb_ff_mul(x_pow, x_val);
    }
  }

  int error_values[MATRIX_SIZE];

  if (tb_ff_solve(error_values, c, syndrome, error_count) != SUCCESS) {
    return !SUCCESS;
  }

  int corrected_coeffs[MATRIX_SIZE];
  memcpy(corrected_coeffs, coeffs, c_size * sizeof(*corrected_coeffs));

  for (int i = 0; i < error_count; i++) {
    corrected_coeffs[error_index[i]] = tb_ff_add(corrected_coeffs[error_index[i]], tb_ff_negate(error_values[i]));
  }

  int corrected_syndrome[MATRIX_SIZE];

  if (tb_ff_rs_decode_raw(data, d_size, corrected_syndrome, corrected_coeffs, c_size) != SUCCESS) {
    return !SUCCESS;
  }

  int exp_syndrome[MATRIX_SIZE] = {0};

  if (memcmp(corrected_syndrome, exp_syndrome, s_size * sizeof(*corrected_syndrome)) != 0) {
    return !SUCCESS;
  }

  return SUCCESS;
}

int tb_ff_rs_find_locations(int d_size, int* error_count, int error_locations[], int syndrome[], int coeffs[], int c_size) {
  if (c_size < d_size) {
    return !SUCCESS;
  }

  int s_size = c_size - d_size;

  int exp_syndrome[MATRIX_SIZE] = {0};

  if (memcmp(syndrome, exp_syndrome, s_size * sizeof(*syndrome)) == 0) {
    *error_count = 0;
    return SUCCESS;
  }

  int max_errors = s_size / 2;

  int c[MATRIX_SIZE][MATRIX_SIZE];

  for (int row = 0; row < max_errors; row++) {
    for (int col = 0; col < max_errors; col++) {
      c[row][col] = syndrome[row + col];
    }
  }

  int alpha[MATRIX_SIZE];

  for (int e = max_errors; e > 0; e--) {
    int neg_syndrome[MATRIX_SIZE];
    for (int i = e; i < s_size; i++) {
      neg_syndrome[i - e] = tb_ff_negate(syndrome[i]);
    }

    if (tb_ff_solve(alpha, c, neg_syndrome, e) != SUCCESS) {
      continue;
    }

    int alpha_rev[MATRIX_SIZE];
    alpha_rev[0] = 1;
    for (int i = 0; i < e; i++) {
      alpha_rev[e - i] = alpha[i];
    }

    int roots[MATRIX_SIZE];

    if (tb_ff_solve_polynomial(roots, alpha_rev, e) != SUCCESS) {
      return !SUCCESS;
    }

    for (int i = 0; i < e; i++) {
      error_locations[i] = tb_ff_log2(tb_ff_inverse(roots[i]));
    }

    *error_count = e;
    return SUCCESS;
  }

  return !SUCCESS;
}
