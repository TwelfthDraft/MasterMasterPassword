#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tb_crypt_lib.h"
#include "tb_constants.h"
#include "tb_test_defines.h"

#include "tb_text_lib_test.h"

int check_tb_new_digest();

int test_crypt_lib() {
  printf("Testing:  crypt_lib\n");

  int pass = 1;

  pass &= check_tb_new_digest() == SUCCESS;

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

  int result = tb_new_text((digest*) (&dst));

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

  TB_TEST_END("tb_new_text:    ");
}
