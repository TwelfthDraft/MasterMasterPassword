#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tb_text_lib.h"
#include "tb_constants.h"
#include "tb_test_defines.h"

int check_tb_strlen();
int check_tb_strlen_single(int*, int*, char*);

int check_tb_settext();
int check_tb_settext_single(int*, int*, char*);

// Utility
void create_str(char*, int);
text* from_padded(padded_text*);
int check_text(text*);

int test_text_lib() {
  printf("Testing:  text_lib\n");

  int pass = 1;

  pass &= check_tb_strlen() == SUCCESS;
  pass &= check_tb_settext() == SUCCESS;

  printf("text_lib: %s\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}

int check_tb_strlen() {
  TEST_START

  create_str(str, 0);                check_tb_strlen_single(&tests, &passes, str);
  create_str(str, 1);                check_tb_strlen_single(&tests, &passes, str);
  create_str(str, 2);                check_tb_strlen_single(&tests, &passes, str);
  create_str(str, 50);               check_tb_strlen_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE - 2); check_tb_strlen_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE - 1); check_tb_strlen_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE);     check_tb_strlen_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE + 1); check_tb_strlen_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE + 2); check_tb_strlen_single(&tests, &passes, str);

  TEST_END("tb_strlen:   ");
}

int check_tb_strlen_single(int* tests, int* passes, char* str) {
  SINGLE_TEST_START

  if (str == NULL) {
    return !SUCCESS;
  }

  int len = strlen(str);

  int exp = len > TB_TEXT_SIZE ? TB_TEXT_SIZE : len;

  int result = tb_strlen(str);

  if (result != exp) {
    return !SUCCESS;
  }

  SINGLE_TEST_END
}

int check_tb_settext() {
  TEST_START

  create_str(str, 0);                check_tb_settext_single(&tests, &passes, str);
  create_str(str, 1);                check_tb_settext_single(&tests, &passes, str);
  create_str(str, 2);                check_tb_settext_single(&tests, &passes, str);
  create_str(str, 50);               check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE - 2); check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE - 1); check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE);     check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE + 1); check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE + 2); check_tb_settext_single(&tests, &passes, str);

  TEST_END("tb_settext:  ");
}

int check_tb_settext_single(int* tests, int* passes, char* str) {
  SINGLE_TEST_START

  if (str == NULL) {
    return !SUCCESS;
  }

  int len = strlen(str);

  int exp = len >= TB_TEXT_SIZE ? (!SUCCESS) : SUCCESS;

  padded_text _dst;
  text* dst = from_padded(&_dst);
  int result = tb_settext(dst, str);

  if (check_text(dst) != SUCCESS) {
    return !SUCCESS;
  }

  if (result != exp) {
    return !SUCCESS;
  }

  if (exp == SUCCESS) {
    if (strncmp(dst->text, str, TB_TEXT_SIZE) != 0) {
      return !SUCCESS;
    }
  }

  SINGLE_TEST_END
}

void create_str(char* str, int len) {
  for (int i = 0; i < len; i++) {
    str[i] = i;
    if (str[i] == 0) {
      str[i]++;
    }
  }
  str[len] = 0;
}

text* from_padded(padded_text* padded) {
  strcpy(padded->text + TB_TEXT_SIZE, padding);
  return (text*) padded;
}

int check_text(text* x) {
  if (x->text[TB_TEXT_SIZE - 1] != 0) {
    return !SUCCESS;
  }

  if (strlen(x->text) >= TB_TEXT_SIZE) {
    return !SUCCESS;
  }

  padded_text* padded = (padded_text*) x;
  if (strcmp(padded->text + TB_TEXT_SIZE, padding) != 0) {
    return !SUCCESS;
  }

  return SUCCESS;
}
