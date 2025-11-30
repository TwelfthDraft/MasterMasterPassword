#include <stdio.h>

#include "tb_constants.h"

#include "tb_crypt_lib.h"

#define LONESHA256_IMPLEMENTATION 1
#include "lonesha256/lonesha256.h"

int tb_new_digest(digest* digest) {
  int result = tb_new_text(digest);
  return result;
}

int tb_digest_to_hex(text* dst, digest* src) {
  return tb_text_to_hex(dst, src, DIGEST_SIZE);
}

int tb_hex_to_digest(digest* dst, text* src) {
  if (tb_strlen(src->text) != DIGEST_SIZE * 2) {
    return !SUCCESS;
  }
  return tb_hex_to_text(dst, src, DIGEST_SIZE);
}

int tb_text_to_digest(digest* dst, text* src, int src_len) {
  return lonesha256(dst->text, src->text, src_len) == 0 ? SUCCESS : (!SUCCESS);
}
