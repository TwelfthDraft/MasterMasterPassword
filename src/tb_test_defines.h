#ifndef TB_TEST_DEFINES_H
#define TB_TEST_DEFINES_H

#define TEST_START    \
  int passes = 0;            \
  int tests = 0;             \
  char str[TB_TEXT_SIZE + 3];

#define TEST_END(name) \
  printf("%s:  %d / %d (%s)\n", name, passes, tests, passes == tests ? "PASS" : "FAIL"); \
                              \
  if (passes != tests) {      \
    return !SUCCESS;          \
  }                           \
                              \
  return SUCCESS;

#define SINGLE_TEST_START \
  (*tests)++;

#define SINGLE_TEST_END   \
  (*passes)++;            \
  return SUCCESS;


typedef struct _padded_text {
  char text[TB_TEXT_SIZE + 8];
} padded_text;

char* padding = "PADDING";

#endif
