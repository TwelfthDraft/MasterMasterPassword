#ifndef TB_CRYPT_LIB_H
#define TB_CRYPT_LIB_H

#include "tb_text_lib.h"

#define DIGEST_SIZE 32

#define MATRIX_SIZE 64

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

// Prompts user to enter a word and appends the word on success
// A minus '-' symbol is added between words
// Retures SUCCESS if a valid word is entered or the text runs out of space
int _tb_prompt_word(FILE* in, FILE* out, int word_num, text* word, text* words);

// Prompts the user to enter a word and appends the word on success
// This is the same as _tb_prompt_word with stdin and stdout used as in and out
int tb_prompt_word(int word_num, text* word, text* words);

// Finite field maths

// Adds two field elements
// Returns the sum of the two elements
int tb_ff_add(int a, int b);

// Multiplies two field elements
// Returns the product of the two elements
int tb_ff_mul(int a, int b);

// Negates a field element
// Returns the negation of the field element
int tb_ff_negate(int a);

// Inverts a field element
// Returns the multiplicative inverse of the field element, or -1 for zero
int tb_ff_inverse(int a);

// Computes the nth power of the 2 field element
// Returns the nth power
int tb_ff_pow2(int n);

// Computes the log to base 2 of a field element
// Returns the log to base 2, or -1 for zero
int tb_ff_log2(int a);

// Solves a series of simultaneous equations
// Find X where Y = C * X
// Returns SUCCESS if a solution is found
int tb_ff_solve(int x[MATRIX_SIZE], int c[][MATRIX_SIZE], int y[MATRIX_SIZE], int size);

// Evaluates a matrix multiply
// Find Y where Y = C * X
// Returns SUCCESS if a solution is found
int tb_ff_evaluate(int y[MATRIX_SIZE], int c[][MATRIX_SIZE], int x[MATRIX_SIZE], int size);

// Converts a list of data values into a list of polynomial coefficients
// The x coordinates are given in the x array
// Returns SUCCESS on success
int tb_ff_lagrange_general(int coeffs[], int x[], int data[], int size);

// Converts a list of polynomial coefficients into a list of data values
// The x coordinates are given in the x array
// Returns SUCCESS on success
int tb_ff_inv_lagrange_general(int data[], int x[], int coeff[], int size);

// Converts a list of data values into a list of polynomial coefficients
// The x coordinates are the powers of 2
// Returns SUCCESS on success
int tb_ff_lagrange(int coeffs[], int data[], int size);

// Converts a list of polynomial coefficients into a list of data values
// The x coordinates are the powers of 2
// Returns SUCCESS on success
int tb_ff_inv_lagrange(int data[], int coeff[], int size);

#endif
