#include <string.h>

#include "tb_constants.h"
#include "tb_text_lib.h"

#include <stdio.h>

#define BUFFER_COUNT 16

int tostr_counter = 0;
char tostr_buffers[BUFFER_COUNT][TB_TEXT_SIZE];

char char_to_hex(int);

int tb_new_text(text* text) {
  memset(text->text, 0, sizeof(text->text));
  return SUCCESS;
}

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

  if (tb_strlen(src->text) >= size) {
    return !SUCCESS;
  }

  strncpy(dst, src->text, size);

  return SUCCESS;
}

int tb_croptext(text* dst, int off, int len) {
  if (dst->text[TB_TEXT_SIZE - 1] != 0) {
    return !SUCCESS;
  }

  if (off < 0 || len < 0) {
    return !SUCCESS;
  }

  int dst_len = tb_strlen(dst->text);

  if (dst_len < off + len) {
    return !SUCCESS;
  }

  for (int i = 0, j = off; i < len + 1; i++, j++) {
    dst->text[i] = dst->text[j];
  }

  return SUCCESS;
}

char* tb_tostr(text* src) {
  if (src->text[TB_TEXT_SIZE - 1] != 0) {
    return NULL;
  }

  int i = tostr_counter & 0xF;
  tostr_counter++;

  char* buffer = tostr_buffers[i];

  strncpy(buffer, src->text, TB_TEXT_SIZE);

  buffer[TB_TEXT_SIZE - 1] = 0;

  return buffer;
}

int tb_textcat(text* dst, text* src) {
  if (dst->text[TB_TEXT_SIZE - 1] != 0) {
    return !SUCCESS;
  }

  if (src->text[TB_TEXT_SIZE - 1] != 0) {
    return !SUCCESS;
  }

  int dst_len = tb_strlen(dst->text);
  int src_len = tb_strlen(src->text);

  if (dst_len + src_len >= TB_TEXT_SIZE) {
    return !SUCCESS;
  }

  strncat(dst->text, src->text, TB_TEXT_SIZE - dst_len);

  return SUCCESS;
}

int tb_text_to_hex(text* dst, text* src, int src_len) {
  if (src_len == -1) {
    src_len = tb_strlen(src->text);
  }

  if (src_len == 0) {
    dst->text[0] = 0;
    return SUCCESS;
  }

  int src_len_by_2 = src_len * 2;

  if (src_len_by_2 > 255) {
    return !SUCCESS;
  }

  char* from = src->text + src_len - 1;
  char* to = dst->text + src_len_by_2;

  *(to--) = 0;

  int rem = src_len;

  while (rem > 0) {
    char c = *(from--);
    *(to--) = char_to_hex(c);
    *(to--) = char_to_hex(c >> 4);
    rem--;
  }

  return SUCCESS;
}

char char_to_hex(int c) {
  c = c & 0xF;

  if (c < 10) {
    return '0' + c;
  } else {
    return 'a' + c - 10;
  }
}
