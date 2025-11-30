#ifndef TB_CRYPT_LIB_H
#define TB_CRYPT_LIB_H

#include "tb_text_lib.h"

#define DIGEST_SIZE 32

typedef text digest;

// Initializes a digest structure
// Returns !SUCCESS on failure
int tb_new_digest(digest* digest);

#endif
