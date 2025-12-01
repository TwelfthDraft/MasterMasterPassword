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

// Converts a text into a digest with SHA256
// Returns SUCCESS
int tb_text_to_digest(digest* dst, text* src, int src_len);

// Computes the digest of the concatenatation of two input digests
// Returns SUCCECSS
int tb_mix_digests(digest* dst, digest* src1, digest* src2);

// Gets a word from the word list
// Returns NULL for words outside 0-1023 range
char* tb_get_word(int index);

// Finds a word in the word list
// Returns the index of the word, or -1 if the word isn't in the list
int tb_find_word(char* word);

#endif
