#include <stdio.h>

#include "tb_file_lib.h"
#include "tb_constants.h"

void consume_second_newline_character(FILE*, int);

int tb_read_line(FILE* fp, text* dst) {
  int pos = 0;

  while (pos < TB_TEXT_SIZE) {
    int c = fgetc(fp);

    if (c == EOF) {
      if (pos == 0) {
        return !SUCCESS;
      } else if (pos < TB_TEXT_SIZE) {
        dst->text[pos] = 0;
        return SUCCESS;
      } else {
        return !SUCCESS;
      }
    }

    if (c == '\r' || c == '\n' || c == EOF) {
      if (fp != stdin) {
        consume_second_newline_character(fp, c);
      }

      if (pos < TB_TEXT_SIZE) {
        dst->text[pos] = 0;
        return SUCCESS;
      } else {
        return !SUCCESS;
      }
    }

    dst->text[pos++] = c;
  }

  while (1) {
    int c = fgetc(fp);
    if (c == EOF || c == '\n' || c == '\r') {
      if (fp != stdin) {
        consume_second_newline_character(fp, c);
      }
      return !SUCCESS;
    }
  }
}

void consume_second_newline_character(FILE* fp, int first) {
  if (first == EOF) {
    return;
  }

  int exp = first == '\n' ? '\r' : '\n';

  int c = fgetc(fp);

  if (c != exp && c != EOF) {
    ungetc(c, fp);
  }
}
