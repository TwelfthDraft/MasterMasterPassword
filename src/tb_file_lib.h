#ifndef TB_FILE_LIB_H
#define TB_FILE_LIB_H

#include "tb_text_lib.h"

// Reads a line from a file into a text
// Returns !SUCCESS on failure and if line is to long
int tb_read_line(FILE*, text*);

// Writes a text to a file
// Returns !SUCCESS on failure
int tb_write_line(FILE*, text);

#endif
