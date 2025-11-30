#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tb_crypt_lib.h"
#include "tb_constants.h"
#include "tb_test_defines.h"

#include "tb_text_lib_test.h"

int check_tb_new_digest();

int check_tb_hex_to_digest();
int check_tb_hex_to_digest_single(int* tests, int* passes, int length, int hex_only);


int test_crypt_lib() {
  printf("Testing:  crypt_lib\n");

  int pass = 1;

  pass &= check_tb_new_digest() == SUCCESS;

  pass &= check_tb_hex_to_digest() == SUCCESS;

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
