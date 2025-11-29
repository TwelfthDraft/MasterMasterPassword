#ifndef TB_TEXT_LIB_TEST
#define TB_TEXT_LIB_TEST

#include "tb_test_defines.h"
#include "tb_text_lib.h"

typedef char padded_str[TB_TEXT_SIZE + TB_STR_MARGIN + TB_PADDING_SIZE];

typedef struct _padded_text {
  char text[TB_TEXT_SIZE + TB_STR_MARGIN + TB_PADDING_SIZE];
} padded_text;

int test_text_lib();

void create_str(char*, int);
text* text_from_padded(padded_text*);
char* str_from_padded(padded_str, int);
int check_str(char*,int);
int check_text(text*);

extern int test_str_lengths[];

#endif
