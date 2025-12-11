#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tb_crypt_lib.h"
#include "tb_constants.h"
#include "tb_test_defines.h"

#include "tb_text_lib_test.h"

#include "tb_words.h"

#define TB_CRYPT_LIB_VECTORS_IMPLEMENTATION
#include "tb_crypt_lib_vectors.h"

int check_tb_new_digest();

int check_tb_digest_to_hex();
int check_tb_digest_to_hex_single(int* tests, int* passes, int length);

int check_tb_hex_to_digest();
int check_tb_hex_to_digest_single(int* tests, int* passes, int length, int hex_only);

int check_tb_text_to_digest();

int check_tb_mix_digests();

int check_tb_get_word();

int check_tb_find_word();

int check_tb_prompt_word();

int check_tb_ff_properties();

int check_tb_ff_evaluate();

int check_tb_ff_solve();

int check_tb_inv_lagrange();

int check_tb_lagrange();

int check_tb_inv_lagrange_general();

int check_tb_lagrange_general();

int check_tb_rs_encode();

int check_tb_rs_decode_raw();

int test_crypt_lib() {
  printf("Testing:  crypt_lib\n");

  int pass = 1;

  pass &= check_tb_new_digest() == SUCCESS;
  pass &= check_tb_digest_to_hex() == SUCCESS;
  pass &= check_tb_hex_to_digest() == SUCCESS;
  pass &= check_tb_text_to_digest() == SUCCESS;
  pass &= check_tb_mix_digests() == SUCCESS;
  pass &= check_tb_get_word() == SUCCESS;
  pass &= check_tb_find_word() == SUCCESS;
  pass &= check_tb_prompt_word() == SUCCESS;
  pass &= check_tb_ff_properties() == SUCCESS;
  pass &= check_tb_ff_evaluate() == SUCCESS;
  pass &= check_tb_ff_solve() == SUCCESS;
  pass &= check_tb_inv_lagrange() == SUCCESS;
  pass &= check_tb_lagrange() == SUCCESS;
  pass &= check_tb_inv_lagrange_general() == SUCCESS;
  pass &= check_tb_lagrange_general() == SUCCESS;
  pass &= check_tb_rs_encode() == SUCCESS;
  pass &= check_tb_rs_decode_raw() == SUCCESS;

  printf("crypt_lib: %s\n\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}

int check_tb_new_digest() {
  TB_TEST_START

  tests++;

  int pass = 1;

  padded_text dst;
  for (int i = 0; i < sizeof(padded_text); i++) {
    dst.text[i] = 1;
  }

  int exp = SUCCESS;

  int result = tb_new_digest((digest*) (&dst));

  if (result != exp) {
    pass = 0;
  }

  for (int i = 0; i < sizeof(padded_text); i++) {
    exp = i < TB_TEXT_SIZE ? 0 : 1;
    if (dst.text[i] != exp) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_new_digest:             ");
}

int check_tb_digest_to_hex() {
  TB_TEST_START

  padded_str padded;

  int lengths[] = {DIGEST_SIZE - 2, DIGEST_SIZE - 1, DIGEST_SIZE, DIGEST_SIZE + 1, DIGEST_SIZE + 2, -1};

  for (int i = 0; lengths[i] != -1; i++) {
    check_tb_digest_to_hex_single(&tests, &passes, lengths[i]);
  }

  TB_TEST_END("tb_digest_to_hex:          ");
}

int check_tb_digest_to_hex_single(int* tests, int* passes, int length) {
  TB_SINGLE_TEST_START

  padded_text _src;
  digest* src = text_from_padded(&_src);
  tb_new_text(src);

  padded_text _dst;
  text* dst = text_from_padded(&_dst);
  tb_new_digest(dst);

  padded_text _dst2;
  text* dst2 = text_from_padded(&_dst2);
  tb_new_text(dst2);

  int exp = SUCCESS;

  for (int i = 0; i < length; i++) {
    src->text[i] = i;
  }

  int result = tb_digest_to_hex(dst, src);

  if (result != exp) {
    return !SUCCESS;
  }

  if (exp == SUCCESS) {
    tb_text_to_hex(dst2, src, DIGEST_SIZE);
    for (int i = 0; i < DIGEST_SIZE * 2; i++) {
      if (dst->text[i] != dst2->text[i]) {
        return !SUCCESS;
      }
    }
  }

  TB_SINGLE_TEST_END
}


int check_tb_hex_to_digest() {
  TB_TEST_START

  padded_str padded;

  int lengths[] = {DIGEST_SIZE - 2, DIGEST_SIZE - 1, DIGEST_SIZE, DIGEST_SIZE + 1, DIGEST_SIZE + 2, -1};

  for (int i = 0; lengths[i] != -1; i++) {
    for (int j = 0; j < 2; j++) {
      check_tb_hex_to_digest_single(&tests, &passes, lengths[i], j);
    }
  }

  TB_TEST_END("tb_hex_to_digest:          ");
}

int check_tb_hex_to_digest_single(int* tests, int* passes, int length, int hex_only) {
  TB_SINGLE_TEST_START

  padded_text _src;
  text* src = text_from_padded(&_src);
  tb_new_text(src);

  padded_text _dst;
  digest* dst = text_from_padded(&_dst);
  tb_new_digest(dst);

  padded_text _dst2;
  text* dst2 = text_from_padded(&_dst2);
  tb_new_text(dst2);

  int exp = length == DIGEST_SIZE && hex_only ? SUCCESS : (!SUCCESS);

  char hex_chars[] = "0123456789ABCDEFabcdef";

  for (int i = 0; i < length * 2; i++) {
    src->text[i] = hex_chars[i % 7]; // (sizeof(hex_chars) - 1)];
  }

  if (!hex_only) {
    src->text[0] = 'g';
  }

  src->text[length * 2] = 0;

  int result = tb_hex_to_digest(dst, src);

  if (result != exp) {
    return !SUCCESS;
  }

  if (exp == SUCCESS) {
    tb_hex_to_text(dst2, src, DIGEST_SIZE);
    for (int i = 0; i < DIGEST_SIZE; i++) {
      if (dst->text[i] != dst2->text[i]) {
        return !SUCCESS;
      }
    }
  }

  TB_SINGLE_TEST_END
}

int check_tb_text_to_digest() {
  TB_TEST_START

  tests++;

  int pass = 1;

  for (int i = 0, j = 0; sha256_vector_lengths[i] != -1; i++, j += 2) {
    int length = sha256_vector_lengths[i];

    char* message = sha256_vectors[j];
    char* exp_digest_hex = sha256_vectors[j + 1];

    digest dst;
    text src;

    memcpy(src.text, message, length);

    int result = tb_text_to_digest(&dst, &src, length);

    if (result != SUCCESS) {
      pass = 0;
      break;
    }

    result = tb_digest_to_hex(&src, &dst);

    if (result != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(src.text, exp_digest_hex, DIGEST_SIZE * 2) != 0) {
      pass = 0;
      break;
    }

  }

  passes += pass;

  TB_TEST_END("tb_text_to_digest:         ");
}

int check_tb_mix_digests() {
  TB_TEST_START

  tests++;

  int pass = 1;

  for (int i = 0, j = 0; i < 8; i++, j += 3) {
    char* dig1 = mix_vectors[j];
    char* dig2 = mix_vectors[j + 1];
    char* exp_digest_hex = mix_vectors[j + 2];

    digest dst;
    digest src1;
    digest src2;

    memcpy(src1.text, dig1, DIGEST_SIZE);
    memcpy(src2.text, dig2, DIGEST_SIZE);

    int result = tb_mix_digests(&dst, &src1, &src2);

    if (result != SUCCESS) {
      pass = 0;
      break;
    }

    result = tb_digest_to_hex(&src1, &dst);

    if (result != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(src1.text, exp_digest_hex, DIGEST_SIZE * 2) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_mix_digests:            ");
}

int check_tb_get_word() {
  TB_TEST_START

  tests++;

  int pass = 1;

  for (int i = -50; i < 1024 + 50; i++) {
    char* exp = (i < 0 || i > 1023) ? NULL : tb_words[i];

    char* actual = tb_get_word(i);

    if (exp == NULL && actual == NULL) {
      continue;
    }

    if (exp == NULL || actual == NULL || strcmp(exp, tb_get_word(i)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_get_word:               ");
}

int check_tb_find_word() {
  TB_TEST_START

  tests++;

  int pass = 1;

  for (int i = 0; i < 1024; i++) {
    int index = tb_find_word(tb_words[i]);

    if (index != i) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  tests++;

  pass = 1;

  for (int i = 0; pass && i < 1024; i++) {
    char* word = tb_words[i];
    int word_len = strlen(word);

    for (int j = 0; j < word_len; j++) {
      char word_copy[6];
      strcpy(word_copy, word);
      word_copy[j]++;

      int index = tb_find_word(word_copy);

      if (index != -1 && strncmp(word_copy, tb_words[index], 5) != 0) {
          pass = 0;
          break;
      }
    }
  }

  passes += pass;

  TB_TEST_END("tb_find_word:              ");
}

int check_tb_prompt_word() {
  TB_TEST_START

  tests++;

  int pass = 0;

  FILE* in = tmpfile();
  FILE* out = tmpfile();

  if (out != NULL && in != NULL) {
    pass = 1;

    int vectors = 1024 + 26 + 1 + 4 + 1;
    char test_vectors[vectors][6];
    int exp[vectors];

    int j = 0;

    for (int i = 0; i < 1024; i++) {
      sprintf(test_vectors[j], "%s", tb_words[i]);
      exp[j] = SUCCESS;
      j++;
    }

    for (int i = 0; i < 26; i++) {
      char c = 'a' + i;
      sprintf(test_vectors[j], "%c%c%c%c%c", c, c, c, c, c);
      exp[j] = !SUCCESS;
      j++;
    }

    sprintf(test_vectors[j], "%s", tb_words[0]);
    exp[j] = SUCCESS;
    j++;


    for (int i = 0; i < 4; i++) {
      test_vectors[j][0] = 0;
      exp[j] = !SUCCESS;
      j++;
    }

    sprintf(test_vectors[j], "%s", tb_words[1]);
    exp[j] = SUCCESS;
    j++;

    char* newlines[] = {"\n", "\r", "\n\r", "\r\n"};

    for (int k = 0; k < 4; k++) {
      for (int i = 0; i < vectors; i++) {
        char* newline = newlines[k];
        fprintf(in, "%s%s", test_vectors[i], newline);
      }
    }

    rewind(in);

    if (vectors != j) {
      pass = 0;
    }

    text minus;
    tb_new_text(&minus);
    minus.text[0] = '-';
    minus.text[1] = 0;

    text words;
    tb_new_text(&words);

    for (int k = 0; k < 4 && pass; k++) {
      for (int i = 0; i < vectors && pass; i++) {
        text word;
        tb_new_text(&word);

        if ((i % 20) == 0) {
          tb_new_text(&words);
        }

        text old_words;
        tb_new_text(&old_words);
        strncpy(old_words.text, words.text, TB_TEXT_SIZE);

        int result = _tb_prompt_word(in, out, i, &word, &words);

        text exp_words;
        tb_new_text(&exp_words);

        if (exp[i] == SUCCESS) {
          if (old_words.text[0] == 0) {
            if (tb_textcat(&exp_words, &words) != SUCCESS) {
              pass = 0;
              break;
            }
          } else {
            if (tb_textcat(&exp_words, &old_words) != SUCCESS ||
                tb_textcat(&exp_words, &minus) != SUCCESS ||
                tb_textcat(&exp_words, &word) != SUCCESS) {
              pass = 0;
              break;
            }
          }
        } else {
          if (tb_textcat(&exp_words, &old_words) != SUCCESS) {
            pass = 0;
            break;
          }
        }

        if (result != exp[i]) {
          pass = 0;
          break;
        }

        if (exp[i] == SUCCESS) {
          if (strncmp(test_vectors[i], word.text, 5) != 0) {
            pass = 0;
            break;
          }
          if (strncmp(words.text, exp_words.text, TB_TEXT_SIZE) != 0) {
            pass = 0;
            break;
          }
        }
      }
    }
  }

  if (in != NULL) {
    fclose(in);
  }

  if (out != NULL) {
    fclose(out);
  }

  passes += pass;

  TB_TEST_END("tb_prompt_word:            ");
}

int check_tb_ff_properties() {
  TB_TEST_START

  tests++;

  int pass = 1;

  int one = 1;
  int zero = 0;

  int step = 16;

  for (int a = 0; pass && a < 1024; a += step) {
    if (tb_ff_mul(a , one) != a) {
      pass = 0;
      break;
    }
    if (tb_ff_mul(a, zero) != 0) {
      pass = 0;
      break;
    }
    if (tb_ff_add(tb_ff_negate(a), a) != zero) {
      pass = 0;
      break;
    }
    if (a != 0 && tb_ff_mul(tb_ff_inverse(a), a) != one) {
      pass = 0;
      break;
    }

    for (int b = 0; pass && b < 1024; b += step) {
      if (tb_ff_mul(a, b) != tb_ff_mul(b, a)) {
        pass = 0;
        break;
      }
      if (tb_ff_add(a, b) != tb_ff_add(b, a)) {
        pass = 0;
        break;
      }

      for (int c = 0; pass && c < 1024; c += step) {
        if (tb_ff_mul(a, tb_ff_mul(b, c)) != tb_ff_mul(tb_ff_mul(a, b), c)) {
          pass = 0;
          break;
        }
        if (tb_ff_add(a, tb_ff_add(b, c)) != tb_ff_add(tb_ff_add(a, b), c)) {
          pass = 0;
          break;
        }
        if (tb_ff_mul(a, tb_ff_add(b, c)) != tb_ff_add(tb_ff_mul(a, b), tb_ff_mul(a, c))) {
          pass = 0;
          break;
        }
      }
    }
  }

  passes += pass;

  tests++;

  pass = 1;

  if (tb_ff_log2(0) != -1) {
    pass = 0;
  }

  int product = 1;
  for (int i = 0; pass && i < 1023; i++) {
    if (tb_ff_log2(product) != i) {
      pass = 0;
      break;
    }
    if (tb_ff_pow2(i) != product) {
      pass = 0;
      break;
    }
    product = tb_ff_mul(product, 2);
  }

  passes += pass;

  tests++;

  product = 1;
  int inv2 = tb_ff_inverse(2);

  for (int i = 0; i < 1023; i++) {
    if (tb_ff_pow2(-i) != product) {
      pass = 0;
      break;
    }
    product = tb_ff_mul(product, inv2);
  }

  passes += pass;

  TB_TEST_END("tb_ff_properties:          ");
}

int check_tb_ff_evaluate() {
  TB_TEST_START

  tests++;

  int pass = 1;

  for (int i = 0; i < 50; i++) {
    int size = (i % 10) + 10;

    int c[MATRIX_SIZE][MATRIX_SIZE];

    int y[MATRIX_SIZE];

    int x[MATRIX_SIZE];

    for (int row = 0; row < size; row++) {
      x[row] = rand() % 1023;
      for (int col = 0; col < size; col++) {
        c[row][col] = rand() % 1023;
      }
    }

    int yExp[MATRIX_SIZE];

    for (int row = 0; row < size; row++) {
      yExp[row] = 0;
    }

    for (int row = 0; row < size; row++) {
      for (int col = 0; col < size; col++) {
        yExp[row] = tb_ff_add(yExp[row], tb_ff_mul(c[row][col], x[col]));
      }
    }

    if (tb_ff_evaluate(y, c, x, size) != SUCCESS) {
      pass = 0;
      break;
    }

    for (int row = 0; row < size; row++) {
      if (yExp[row] != y[row]) {
        pass = 0;
        break;
      }
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_evaluate:            ");
}

int check_tb_ff_solve() {
  TB_TEST_START

  tests++;

  int pass = 1;

  srand(12345);

  for (int i = 0; i < 50; i++) {
    int size = (i % 10) + 10;

    int c[MATRIX_SIZE][MATRIX_SIZE];

    int y[MATRIX_SIZE];

    int x[MATRIX_SIZE];

    for (int row = 0; row < size; row++) {
      y[row] = rand() % 1023;
      for (int col = 0; col < size; col++) {
        c[row][col] = rand() % 1023;
      }
    }

    if (tb_ff_solve(x, c, y, size) != SUCCESS) {
      pass = 0;
      break;
    }

    int result[MATRIX_SIZE];

    if (tb_ff_evaluate(result, c, x, size) != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(result, y, size * sizeof(*result)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_solve:               ");
}

int check_tb_inv_lagrange() {
  TB_TEST_START

  tests++;

  int pass = 1;

  srand(13579);

  for (int i = 0; i < 50; i++) {
    int size = (i % 10) + 10;

    int coeffs[MATRIX_SIZE];

    for (int j = 0; j < size; j++) {
      coeffs[j] = rand() % 1023;
    }

    int exp_data[MATRIX_SIZE];

    for (int j = 0; j < size; j++) {
      int sum = 0;
      for (int k = 0; k < size; k++) {
        int x_j_k = tb_ff_pow2(j * k);
        int t_j_k = tb_ff_mul(x_j_k, coeffs[k]);
        sum = tb_ff_add(sum, t_j_k);
      }
      exp_data[j] = sum;
    }

    int data[MATRIX_SIZE];

    if (tb_ff_inv_lagrange(data, coeffs, size) != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(exp_data, data, size * sizeof(*exp_data)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_inv_lagrange:        ");
}

int check_tb_lagrange() {
  TB_TEST_START

  tests++;

  int pass = 1;

  srand(24680);

  for (int i = 0; i < 50; i++) {
    int size = (i % 10) + 10;

    int data[MATRIX_SIZE];

    for (int j = 0; j < size; j++) {
      data[j] = rand() % 1023;
    }

    int coeffs[MATRIX_SIZE];

    if (tb_ff_lagrange(coeffs, data, size) != SUCCESS) {
      pass = 0;
      break;
    }

    int inv_data[MATRIX_SIZE];

    if (tb_ff_inv_lagrange(inv_data, coeffs, size) != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(inv_data, data, size * sizeof(*inv_data)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_lagrange:            ");
}

int check_tb_inv_lagrange_general() {
  TB_TEST_START

  tests++;

  int pass = 1;

  srand(112233);

  for (int i = 0; i < 50; i++) {
    int size = (i % 10) + 10;

    int x[MATRIX_SIZE];
    int coeffs[MATRIX_SIZE];

    for (int j = 0; j < size; j++) {
      coeffs[j] = rand() % 1023;
      x[j] = rand() % 1023;
      for (int k = 0; k < j; k++) {
        if (x[j] == x[k]) {
          j--;
          break;
        }
      }
    }

    int exp_data[MATRIX_SIZE];

    for (int j = 0; j < size; j++) {
      int sum = 0;
      int x_j_k = 1;
      for (int k = 0; k < size; k++) {
        int t_j_k = tb_ff_mul(x_j_k, coeffs[k]);
        sum = tb_ff_add(sum, t_j_k);
        x_j_k = tb_ff_mul(x_j_k, x[j]);
      }
      exp_data[j] = sum;
    }

    int data[MATRIX_SIZE];

    if (tb_ff_inv_lagrange_general(data, x, coeffs, size) != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(exp_data, data, size * sizeof(*exp_data)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_inv_lagrange_general:");
}

int check_tb_lagrange_general() {
  TB_TEST_START

  tests++;

  int pass = 1;

  srand(445566);

  for (int i = 0; i < 50; i++) {
    int size = (i % 10) + 10;

    int x[MATRIX_SIZE];
    int data[MATRIX_SIZE];

    for (int j = 0; j < size; j++) {
      data[j] = rand() % 1023;
      x[j] = rand() % 1023;
      for (int k = 0; k < j; k++) {
        if (x[j] == x[k]) {
          j--;
          break;
        }
      }
    }

    int coeffs[MATRIX_SIZE];

    if (tb_ff_lagrange_general(coeffs, x, data, size) != SUCCESS) {
      pass = 0;
      break;
    }

    int inv_data[MATRIX_SIZE];

    if (tb_ff_inv_lagrange_general(inv_data, x, coeffs, size) != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(inv_data, data, size * sizeof(*inv_data)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_lagrange_general:    ");
}

int check_tb_rs_encode() {
  TB_TEST_START

  tests++;

  int pass = 1;

  srand(334455);

  for (int i = 0; i < 50; i++) {
    int d_size = (i % 10) + 10;
    int c_size = d_size + (i % 11);

    int data[MATRIX_SIZE];
    int ext_data[MATRIX_SIZE];

    int s_size = c_size - d_size;

    for (int j = 0; j < s_size; j++) {
      ext_data[j] = 0;
    }

    for (int j = 0; j < d_size; j++) {
      data[j] = rand() % 1023;
      ext_data[j + s_size] = data[j];
    }

    int coeffs[MATRIX_SIZE];

    if (tb_ff_rs_encode(coeffs, c_size, data, d_size) != SUCCESS) {
      pass = 0;
      break;
    }

    int inv_data[MATRIX_SIZE];
    if (tb_ff_inv_lagrange(inv_data, coeffs, c_size) != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(inv_data, ext_data, c_size * sizeof(*inv_data)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_rs_encode:           ");
}

int check_tb_rs_decode_raw() {
  TB_TEST_START

  tests++;

  int pass = 1;

  srand(334455);

  for (int i = 0; i < 50; i++) {
    int d_size = (i % 10) + 10;
    int c_size = d_size + (i % 11);

    int data[MATRIX_SIZE];

    int s_size = c_size - d_size;

    for (int j = 0; j < d_size; j++) {
      data[j] = rand() % 1023;
    }

    int coeffs[MATRIX_SIZE];

    if (tb_ff_rs_encode(coeffs, c_size, data, d_size) != SUCCESS) {
      pass = 0;
      break;
    }

    int inv_data[MATRIX_SIZE];
    int syn[MATRIX_SIZE];
    int exp_syn[MATRIX_SIZE] = {0};

    if (tb_ff_rs_decode_raw(inv_data, d_size, syn, coeffs, c_size) != SUCCESS) {
      pass = 0;
      break;
    }

    if (memcmp(inv_data, data, d_size * sizeof(*inv_data)) != 0) {
      pass = 0;
      break;
    }

    if (memcmp(syn, exp_syn, s_size * sizeof(*syn)) != 0) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_ff_rs_decode_raw:       ");
}
