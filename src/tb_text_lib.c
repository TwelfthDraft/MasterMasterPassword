#include <string.h>

#include "tb_constants.h"
#include "tb_text_lib.h"

int tb_settext(text* dst, char* src) {
  if (strlen(src) >= TB_TEXT_SIZE) {
    return !SUCCESS;
  }

  strncpy(dst->text, src, TB_TEXT_SIZE);

  return SUCCESS;
}

int tb_gettext(char* dst, size_t size, text* src);

int tb_croptext(text* dst, int off, int len);

char* tb_tostr(text* src);

int tb_textcat(text* dst, text* src);

