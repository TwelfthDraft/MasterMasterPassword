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

  TB_TEST_END("tb_new_digest:    ");
}

int check_tb_digest_to_hex() {
  TB_TEST_START

  padded_str padded;

  int lengths[] = {DIGEST_SIZE - 2, DIGEST_SIZE - 1, DIGEST_SIZE, DIGEST_SIZE + 1, DIGEST_SIZE + 2, -1};

  for (int i = 0; lengths[i] != -1; i++) {
    check_tb_digest_to_hex_single(&tests, &passes, lengths[i]);
  }

  TB_TEST_END("tb_digest_to_hex: ");
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

  TB_TEST_END("tb_hex_to_digest: ");
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

  TB_TEST_END("tb_text_to_digest:");
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

  TB_TEST_END("tb_mix_digests:   ");
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

  TB_TEST_END("tb_get_word:      ");
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

  TB_TEST_END("tb_find_word:     ");
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

  if (in == NULL) {
    fclose(in);
  }

  if (out == NULL) {
    fclose(out);
  }

  passes += pass;

  TB_TEST_END("tb_prompt_word:   ");
}
