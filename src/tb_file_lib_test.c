#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tb_text_lib.h"
#include "tb_constants.h"
#include "tb_test_defines.h"

#include "tb_text_lib_test.h"

int tb_read_line(FILE* fp, text* dst);

int check_tb_read_line();
int check_tb_read_line_single(int*, int*);

int test_file_lib() {
  printf("Testing:  file_lib\n");

  int pass = 1;

  pass &= check_tb_read_line() == SUCCESS;

  printf("file_lib: %s\n\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}


int check_tb_read_line() {
  TB_TEST_START

  check_tb_read_line_single(&tests, &passes);

  remove("tmp.dat");

  TB_TEST_END("tb_read_line:  ");
}

int check_tb_read_line_single(int* tests, int* passes) {
  TB_SINGLE_TEST_START

  padded_str _padded_strs[16];
  padded_str* padded_strs = _padded_strs;

  int str_count = 0;
  for (int i = 0; i < 16 && test_str_lengths[i] != -1; i++) {
    create_str(padded_strs[i], test_str_lengths[i]);
    str_count++;
  }

  FILE* fp = fopen("tmp.dat", "wb");
  if (fp == NULL) {
    fprintf(stderr, "Unable to open file for testing\n");
    return !SUCCESS;
  }

  for (int i = 0; i < str_count; i++) {
    fprintf(fp, "%s\n", padded_strs[i]);
  }

  fclose(fp);

  fp = fopen("tmp.dat", "rb");
  if (fp == NULL) {
    fprintf(stderr, "Unable to open file for testing\n");
    return !SUCCESS;
  }

  for (int i = 0; i < str_count; i++) {
    int exp = strlen(padded_strs[i]) >= TB_TEXT_SIZE ? (!SUCCESS) : SUCCESS;

    padded_text _dst;
    tb_new_text((text*) (&_dst));
    text* dst = text_from_padded(&_dst);
    _dst.text[TB_TEXT_SIZE - 1] = 0;

    int result = tb_read_line(fp, dst);

    if (exp != result) {
      fclose(fp);
      return !SUCCESS;
    }

    if (exp == SUCCESS) {
      if (strcmp(padded_strs[i], dst->text) != 0) {
        fclose(fp);
        return !SUCCESS;
      }
    }
  }

  fclose(fp);

  TB_SINGLE_TEST_END
}

