#ifndef TB_TEST_DEFINES_H
#define TB_TEST_DEFINES_H

#define TB_TEST_START    \
  int passes = 0;            \
  int tests = 0;

#define TB_TEST_END(name) \
  printf("%s:  %d / %d (%s)\n", name, passes, tests, passes == tests ? "PASS" : "FAIL"); \
                              \
  if (passes != tests) {      \
    return !SUCCESS;          \
  }                           \
                              \
  return SUCCESS;

#define TB_SINGLE_TEST_START \
  (*tests)++;

#define TB_SINGLE_TEST_END   \
  (*passes)++;            \
  return SUCCESS;

#define TB_STR_MARGIN 3
#define TB_PADDING_SIZE 8

#define TB_PADDING ("PADDING")

typedef char padded_str[TB_TEXT_SIZE + TB_STR_MARGIN + TB_PADDING_SIZE];

typedef struct _padded_text {
  char text[TB_TEXT_SIZE + TB_STR_MARGIN + TB_PADDING_SIZE];
} padded_text;

#endif
