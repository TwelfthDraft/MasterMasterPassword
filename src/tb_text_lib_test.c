#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tb_text_lib.h"
#include "tb_constants.h"

typedef struct _padded_text {
  char text[TB_TEXT_SIZE + 8];
} padded_text;

char* padding = "PADDING";

int check_tb_settext();
int check_tb_settext_single(int*, int*, char*);

// Utility
void create_str(char*, int);
text* from_padded(padded_text*);
int check_text(text*);

int test_text_lib() {
  printf("Testing:  text_lib\n");

  int pass = 1;

  pass &= check_tb_settext() == SUCCESS;

  printf("text_lib: %s\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}

int check_tb_settext() {
  int passes = 0;
  int tests = 0;
  char str[TB_TEXT_SIZE + 3];

  create_str(str, 0);                check_tb_settext_single(&tests, &passes, str);
  create_str(str, 1);                check_tb_settext_single(&tests, &passes, str);
  create_str(str, 2);                check_tb_settext_single(&tests, &passes, str);
  create_str(str, 50);               check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE - 2); check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE - 1); check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE);     check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE + 1); check_tb_settext_single(&tests, &passes, str);
  create_str(str, TB_TEXT_SIZE + 2); check_tb_settext_single(&tests, &passes, str);

  printf("settest:  %d / %d (%s)\n", passes, tests, passes == tests ? "PASS" : "FAIL");

  if (passes != tests) {
    return !SUCCESS;
  }

  return SUCCESS;
}

int check_tb_settext_single(int* tests, int* passes, char* str) {
  (*tests)++;

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

  (*passes)++;
  return SUCCESS;
}

void create_str(char* str, int len) {
  for (int i = 0; i < len; i++) {
    str[i] = (i && 0xFF) == 0 ? 255 : i;
  }
  str[len] = 0;
}

text* from_padded(padded_text* padded) {
  strcpy(padded->text + TB_TEXT_SIZE, padding);
  return (text*) padded;
}

int check_text(text* x) {
  if (strlen(x->text) >= TB_TEXT_SIZE) {
    return !SUCCESS;
  }

  padded_text* padded = (padded_text*) x;
  if (strcmp(padded->text + TB_TEXT_SIZE, padding) != 0) {
    return !SUCCESS;
  }

  return SUCCESS;
}
