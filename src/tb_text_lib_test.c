#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tb_text_lib.h"
#include "tb_constants.h"
#include "tb_test_defines.h"

int test_str_lengths[] = {0, 1, 2, 50, TB_TEXT_SIZE - 2, TB_TEXT_SIZE - 1, TB_TEXT_SIZE, TB_TEXT_SIZE + 1, TB_TEXT_SIZE + 2, -1};

char* padding = TB_PADDING;

int check_tb_new_text();

int check_tb_strlen();
int check_tb_strlen_single(int*, int*, char*);

int check_tb_settext();
int check_tb_settext_single(int*, int*, padded_str);

int check_tb_gettext();
int check_tb_gettext_single(int*, int*, padded_str);

int check_tb_croptext();
int check_tb_croptext_single(int*, int*, padded_str);

// Utility
void create_str(char*, int);
text* text_from_padded(padded_text*);
char* str_from_padded(padded_str, int);
int check_str(char*,int);
int check_text(text*);

int test_text_lib() {
  printf("Testing:  text_lib\n");

  int pass = 1;

  pass &= check_tb_new_text() == SUCCESS;
  pass &= check_tb_strlen() == SUCCESS;
  pass &= check_tb_settext() == SUCCESS;
  pass &= check_tb_gettext() == SUCCESS;
  pass &= check_tb_croptext() == SUCCESS;

  printf("text_lib: %s\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}

int check_tb_new_text() {
  TB_TEST_START

  tests++;

  int pass = 1;

  padded_text dst;
  for (int i = 0; i < sizeof(padded_text); i++) {
    dst.text[i] = 1;
  }

  int exp = SUCCESS;

  int result = tb_new_text((text*) (&dst));

  if (result != exp) {
    pass = 0;
  }

  for (int i = 0; i < sizeof(padded_text); i++) {
    exp = i == (TB_TEXT_SIZE - 1) ? 0 : 1;
    if (dst.text[i] != exp) {
      pass = 0;
      break;
    }
  }

  passes += pass;

  TB_TEST_END("tb_new_text: ");
}

int check_tb_strlen() {
  TB_TEST_START

  padded_str padded;

  for (int i = 0; test_str_lengths[i] != -1; i++) {
    create_str(padded, test_str_lengths[i]);
    check_tb_strlen_single(&tests, &passes, padded);
  }

  TB_TEST_END("tb_strlen:   ");
}

int check_tb_strlen_single(int* tests, int* passes, padded_str padded) {
  TB_SINGLE_TEST_START

  if (padded == NULL) {
    return !SUCCESS;
  }

  char* str = str_from_padded(padded, strlen(padded) + 1);

  int len = strlen(str);

  int exp = len > TB_TEXT_SIZE ? TB_TEXT_SIZE : len;

  int result = tb_strlen(str);

  if (result != exp) {
    return !SUCCESS;
  }

  TB_SINGLE_TEST_END
}

int check_tb_settext() {
  TB_TEST_START

  padded_str padded;

  for (int i = 0; test_str_lengths[i] != -1; i++) {
    create_str(padded, test_str_lengths[i]);
    check_tb_settext_single(&tests, &passes, padded);
  }

  TB_TEST_END("tb_settext:  ");
}

int check_tb_settext_single(int* tests, int* passes, padded_str padded) {
  TB_SINGLE_TEST_START

  if (padded == NULL) {
    return !SUCCESS;
  }

  char* str = str_from_padded(padded, strlen(padded) + 1);

  int len = strlen(str);

  int exp = len >= TB_TEXT_SIZE ? (!SUCCESS) : SUCCESS;

  padded_text _dst;
  tb_new_text((text*) (&_dst));
  text* dst = text_from_padded(&_dst);
  _dst.text[TB_TEXT_SIZE - 1] = 0;
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

  TB_SINGLE_TEST_END
}


int check_tb_gettext() {
  TB_TEST_START

  padded_str padded;

  for (int i = 0; test_str_lengths[i] != -1; i++) {
    create_str(padded, test_str_lengths[i]);
    check_tb_gettext_single(&tests, &passes, padded);
  }

  TB_TEST_END("tb_gettext:  ");
}

int check_tb_gettext_single(int* tests, int* passes, padded_str padded) {
  TB_SINGLE_TEST_START

  if (padded == NULL) {
    return !SUCCESS;
  }

  char* str = str_from_padded(padded, strlen(padded) + 1);

  int len = strlen(str);
  if (len >= TB_TEXT_SIZE) {
    (*passes)++;
    return SUCCESS;
  }

  padded_str _dst;
  char* dst;

  padded_text _src;
  tb_new_text((text*) (&_src));
  text* src = text_from_padded(&_src);
  int result = tb_settext(src, str);

  if (result != SUCCESS) {
    return !SUCCESS;
  }

  for (int i = -1; i < 2; i++) {
    int dst_size = len + i;
    if (dst_size < 0) {
      continue;
    }

    dst = str_from_padded(_dst, dst_size);

    int exp = len >= dst_size ? (!SUCCESS) : SUCCESS;
    result = tb_gettext(dst, dst_size, src);

    if (exp == SUCCESS && check_str(dst, dst_size) != SUCCESS) {
      return !SUCCESS;
    }

    if (check_text(src) != SUCCESS) {
      return !SUCCESS;
    }

    if (result != exp) {
      return !SUCCESS;
    }

    if (exp == SUCCESS) {
      if (strncmp(dst, str, TB_TEXT_SIZE) != 0) {
        return !SUCCESS;
      }
    }
  }

  TB_SINGLE_TEST_END
}

int check_tb_croptext() {
  TB_TEST_START

  padded_str padded;

  for (int i = 0; test_str_lengths[i] != -1; i++) {
    create_str(padded, test_str_lengths[i]);
    check_tb_croptext_single(&tests, &passes, padded);
  }

  TB_TEST_END("tb_croptext:  ");
}

int check_tb_croptext_single(int* tests, int* passes, padded_str padded) {
  TB_SINGLE_TEST_START

  if (padded == NULL) {
    return !SUCCESS;
  }

  char* str = str_from_padded(padded, strlen(padded) + 1);

  int str_len = strlen(str);
  if (str_len >= TB_TEXT_SIZE) {
    (*passes)++;
    return SUCCESS;
  }

  int offs[] = {-2, -1, 0, 1, 2, str_len - 2, str_len - 1, str_len, str_len + 1, str_len + 2, INT_MIN};
  int *ends = offs;

  for (int i = 0; offs[i] != INT_MIN; i++) {
    int off = offs[i];
    for (int j = 0; ends[j] != INT_MIN; j++) {
      int end = ends[j];
      int len = end - off;

      padded_text _dst;
      text* dst = text_from_padded(&_dst);
      tb_new_text((text*) (&_dst));
      int result = tb_settext(dst, str);

      if (result != SUCCESS) {
        return !SUCCESS;
      }

      int exp = (off < 0 || off > str_len || (off + len) < 0 || (off + len) > str_len) || (len < 0) || (len > str_len) ? (!SUCCESS) : SUCCESS;     
      
      result = tb_croptext(dst, off, len);

      if (check_text(dst) != SUCCESS) {
        return !SUCCESS;
      }

      if (result != exp) {
        return !SUCCESS;
      }

      if (exp == SUCCESS) {
        for (int k = 0; k < len; k++) {
          if (str[k + off] != dst->text[k]) {
            return !SUCCESS;
          }
        }
      }
    }
  }
  
  TB_SINGLE_TEST_END
} 

void create_str(char* str, int len) {
  for (int i = 0; i < len; i++) {
    str[i] = i;

    str[i] &= 0x7F;

    if (str[i] < ' ') {
      str[i] += ' ';
    }

    if (str[i] == 0) {
      str[i]++;
    }
  }
  str[len] = 0;
}

text* text_from_padded(padded_text* padded) {
  strcpy(padded->text + TB_TEXT_SIZE, padding);
  return (text*) padded;
}

char* str_from_padded(padded_str padded, int len) {
  strcpy(padded + len, padding);
  return (char*) padded;
}

int check_str(char* str, int size) {
  int len = strlen(str);
  if (len >= TB_TEXT_SIZE + TB_STR_MARGIN) {
    return !SUCCESS;
  }

  if (strcmp(str + size, padding) != 0) {
    return !SUCCESS;
  }

  return SUCCESS;
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
