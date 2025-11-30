#include <stdio.h>

#include "tb_crypt_lib.h"

int tb_new_digest(digest* digest) {
  int result = tb_new_text(digest);
  return result;
}
