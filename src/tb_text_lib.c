#include <string.h>

#include "tb_constants.h"
#include "tb_text_lib.h"

#include <stdio.h>

int tb_strlen(char* str) {
  for (int i = 0; i < TB_TEXT_SIZE; i++) {
    if (str[i] == 0) {
      return i;
    }
  }

  return TB_TEXT_SIZE;
}

int tb_settext(text* dst, char* src) {
  if (dst->text[TB_TEXT_SIZE - 1] != 0) {
    return !SUCCESS;
  }

  if (tb_strlen(src) >= TB_TEXT_SIZE) {
    return !SUCCESS;
  }

  strncpy(dst->text, src, TB_TEXT_SIZE);

  return SUCCESS;
}

int tb_gettext(char* dst, size_t size, text* src) {
  if (src->text[TB_TEXT_SIZE - 1] != 0) {
    return !SUCCESS;
  }

  if (tb_strlen(src->text) >= TB_TEXT_SIZE) {
    return !SUCCESS;
  }

  strncpy(dst, src->text, TB_TEXT_SIZE);

  return SUCCESS;
}

int tb_croptext(text* dst, int off, int len);

char* tb_tostr(text* src);

int tb_textcat(text* dst, text* src);

