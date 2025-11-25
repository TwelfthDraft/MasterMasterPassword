#include <stdio.h>

#include "tb_constants.h"
#include "tb_text_lib_test.h"

int main(int argc, char* argv[]) {
  int pass = 1;

  pass &= test_text_lib() == SUCCESS;

  printf("\n");
  printf("Total: %s\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}
