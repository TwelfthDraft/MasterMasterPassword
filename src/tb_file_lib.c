#include <stdio.h>

#include "tb_file_lib.h"
#include "tb_constants.h"

int tb_read_line(FILE* fp, text* dst) {
  int pos = 0;

  while (pos < TB_TEXT_SIZE) {
    int c = fgetc(fp);

    if (c == EOF) {
      if (pos < TB_TEXT_SIZE) {
        dst->text[pos] = 0;
        return SUCCESS;
      } else {
        return !SUCCESS;
      }
    }

    if (c < ' ') {
      if (pos < TB_TEXT_SIZE) {
        dst->text[pos] = 0;
        return SUCCESS;
      } else {
        return !SUCCESS;
      }
    }

    if (pos >= TB_TEXT_SIZE) {
      return !SUCCESS;
    }

    dst->text[pos++] = c;
  }

  while (1) {
    int c = fgetc(fp);
    if (c == EOF || c < ' ') {
      return !SUCCESS;
    }
  }
}
