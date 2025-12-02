#include <stdio.h>

#include "tb_constants.h"

#include "tb_crypt_lib.h"
#include "tb_file_lib.h"

#define LONESHA256_IMPLEMENTATION 1
#include "lonesha256/lonesha256.h"

#define TB_WORDS_IMPLEMENTATION 1
#include "tb_words.h"

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
