#ifndef TB_CRYPT_LIB_H
#define TB_CRYPT_LIB_H

#include "tb_text_lib.h"

#define DIGEST_SIZE 32

typedef text digest;

// Initializes a digest structure
// Returns !SUCCESS on failure
int tb_new_digest(digest* digest);

// Converts a digest into a text in hex format
// Returns SUCCESS
int tb_digest_to_hex(text* dst, digest* src);

// Converts a text in hex format to a digest
// Returns !SUCCESS if the text doesn't contain a 64 hex character string
int tb_hex_to_digest(digest* dst, text* src);

#endif
