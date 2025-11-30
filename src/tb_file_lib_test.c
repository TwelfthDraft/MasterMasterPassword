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
int check_tb_read_line_single(int*, int*, char*, int);

int test_file_lib() {
  printf("Testing:  file_lib\n");

  int pass = 1;

  pass &= check_tb_read_line() == SUCCESS;

  printf("file_lib: %s\n\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}


int check_tb_read_line() {
  TB_TEST_START

  char* newlines[] = {"\n", "\r", "\r\n", "\n\r", NULL};

  for (int i = 0; newlines[i] != NULL; i++) {
    for (int j = 0; j < 2; j++) {
      check_tb_read_line_single(&tests, &passes, newlines[i], j);
    }
  }

  remove("tmp.dat");

  TB_TEST_END("tb_read_line:     ");
}

int check_tb_read_line_single(int* tests, int* passes, char* newline, int dos_end) {
  TB_SINGLE_TEST_START

  padded_str _padded_strs[16];
  padded_str* padded_strs = _padded_strs;

  int str_count = 0;
  int i;
  for (i = 0; i < 15 && test_str_lengths[i] != -1; i++) {
    create_str(padded_strs[i], test_str_lengths[i]);
    str_count++;
  }
  create_str(padded_strs[i], 50);
  str_count++;

  FILE* fp = fopen("tmp.dat", "wb");
  if (fp == NULL) {
    fprintf(stderr, "Unable to open file for testing\n");
    return !SUCCESS;
  }

  for (int i = 0; i < str_count; i++) {
    fprintf(fp, "%s", padded_strs[i]);
    if ((!dos_end) || i < str_count - 1) {
      fprintf(fp, "%s", newline);
    }
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

    if (result == SUCCESS) {
      if (strcmp(padded_strs[i], dst->text) != 0) {
        fclose(fp);
        return !SUCCESS;
      }
    }
  }

  padded_text _dst;
  tb_new_text((text*) (&_dst));
  text* dst = text_from_padded(&_dst);
  _dst.text[TB_TEXT_SIZE - 1] = 0;

  int result = tb_read_line(fp, dst);

  int exp = !SUCCESS;

  if (result != exp) {
    fclose(fp);
    return !SUCCESS;
  }

  fclose(fp);

  TB_SINGLE_TEST_END
}

