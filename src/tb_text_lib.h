#ifndef TB_TEXT_LIB_H
#define TB_TEXT_LIB_H

#define TB_TEXT_SIZE 256

typedef struct _text {
  char text[TB_TEXT_SIZE];
} text;

// Initializes a text structure
// Returns !SUCCESS on failure
int tb_new_text(text*);

// Gets the length of a string but only checks the first TB_TEXT_SIZE characters
// Returns the length of the string (max TB_TEXT_SIZE)
int tb_strlen(char* str);

// Set a text to a given string
// Returns !SUCCESS if string is to long
int tb_settext(text* dst, char* src);

// Sets a string to a given text
// Returns !SUCCESS if the string has insufficient space
int tb_gettext(char* dst, size_t size, text* src);

// Crops a text from a given offset and length
// Returns !SUCCESS if the text doesn't contain the range
int tb_croptext(text* dst, int off, int len);

// Copies a text into a string
// Note only the most recent 16 return values of this method are reliable
// This method is not thread safe
// Returns the string
char* tb_tostr(text* src);

// Appends one text onto another
// Returns !SUCCESS if there is not sufficient space
int tb_textcat(text* dst, text* src);

#endif
