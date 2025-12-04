#include <stdio.h>

#include "tb_constants.h"

#include "tb_crypt_lib.h"
#include "tb_file_lib.h"

#define LONESHA256_IMPLEMENTATION 1
#include "lonesha256/lonesha256.h"

#define TB_WORDS_IMPLEMENTATION 1
#include "tb_words.h"

int log2_arr[1024] = {-1};
int pow2_arr[1024] = {-1};

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

int tb_mix_digests(digest* dst, digest* src1, digest* src2) {
  text msg;

  memcpy(msg.text, src1->text, DIGEST_SIZE);
  memcpy(msg.text + DIGEST_SIZE, src2->text, DIGEST_SIZE);

  return tb_text_to_digest(dst, &msg, DIGEST_SIZE * 2);
}

char* tb_get_word(int index) {
  if (index < 0 || index >= 1024) {
    return NULL;
  }

  return tb_words[index];
}

int tb_find_word(char* word) {
  if (word == NULL) {
    return -1;
  }

  int start = 0;
  int end = 1023;

  while (start <= end) {
    int mid = (start + end + 1) / 2;

    char* mid_word = tb_words[mid];

    int comp = strncmp(mid_word, word, 5);

    if (comp == 0) {
      return mid;
    }

    if (comp > 0) {
      end = mid - 1;
    } else {
      start = mid + 1;
    }
  }

  return -1;
}

int _tb_prompt_word(FILE* in, FILE* out, int word_num, text* word, text* words) {
  if (in == NULL || out == NULL || feof(in) || feof(out)) {
    return !SUCCESS;
  }

  tb_new_text(word);

  fprintf(out, "Please enter word %d\n", word_num);

  if (tb_read_line(in, word) != SUCCESS) {
    fprintf(out, "Failed to read line\n");
    return !SUCCESS;
  }

  if (tb_find_word(word->text) == -1) {
    fprintf(out, "Unknown word %s\n", word->text);
    return !SUCCESS;
  }

  if (words != NULL) {
    text minus;
    tb_new_text(&minus);
    strncpy(minus.text, "-", 2);

    if (words->text[0] != 0) {
      if (tb_textcat(words, &minus) != SUCCESS) {
        return !SUCCESS;
      }
    }

    if (tb_textcat(words, word) != SUCCESS) {
      return !SUCCESS;
    }
  }

  return SUCCESS;
}

int tb_prompt_word(int word_num, text* word, text* words) {
  return _tb_prompt_word(stdin, stdout, word_num, word, words);
}

// Finite field maths

int MOD = 1033;

int _tb_ff_mul(int a, int b) {
  if (a < 0 || a > 1023 || b < 0 || b > 1023) {
    return -1;
  }

  int product = a * b;

  int shift = 10;
  int mod = MOD << shift;

  while (shift >= 0) {
    int next = mod ^ product;
    if (next < product) {
      product = next;
    }
    mod >>= 1;
    shift--;
  }

  return product;
}

void init_cache() {
  if (pow2_arr[0] != -1) {
    return;
  }

  int g = 2;
  int prod = 1;

  for (int i = 0; i < 1023; i++) {
    pow2_arr[i] = prod;
    log2_arr[prod] = i;
    prod = _tb_ff_mul(prod, g);
  }
}

int tb_ff_add(int a, int b) {
  if (a < 0 || a > 1023 || b < 0 || b > 1023) {
    return -1;
  }

  return a ^ b;
}

int tb_ff_mul(int a, int b) {
  init_cache();

  if (a == 0 || b == 0) {
    return 0;
  }

  if (a < 0 || a > 1023 || b < 0 || b > 1023) {
    return -1;
  }

  int logA = log2_arr[a];
  int logB = log2_arr[b];
  int logAB = (logA + logB) % 1023;

  return pow2_arr[logAB];
}

int tb_ff_negate(int a) {
  if (a < 0 || a > 1023) {
    return -1;
  }

  return a;
}

int tb_ff_inverse(int a) {
  init_cache();

  if (a == 0 || a < 0 || a > 1023) {
    return -1;
  }

  int lg2 = log2_arr[a];
  int index = lg2 == 0 ? 0 : (1023 - lg2);

  return pow2_arr[index];
}

int tb_ff_pow2(int n) {
  init_cache();

  if (n < 0 || n >= 1023) {
    n = n % 1023;
    while (n < 0) {
      n += 1023;
    }
  }

  return pow2_arr[n];
}

int tb_ff_log2(int a) {
  init_cache();

  if (a == 0 || a < 0 || a > 1023) {
    return -1;
  }

  return log2_arr[a];
}

