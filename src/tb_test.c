#include <stdio.h>

#include "tb_constants.h"
#include "tb_text_lib_test.h"
#include "tb_file_lib_test.h"

int main(int argc, char* argv[]) {
  int pass = 1;

  pass &= test_text_lib() == SUCCESS;
  pass &= test_file_lib() == SUCCESS;

  printf("Total: %s\n", pass ? "PASS" : "FAIL");

  return pass ? SUCCESS : (!SUCCESS);
}
