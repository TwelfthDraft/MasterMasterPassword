# Licensed under MIT license, see LICENSE file #

CC=gcc
CFLAGS=
INCFLAGS=-Isrc -Isrc/lonesha256/

OBJDIR=obj
SRCDIR=src

all:

sha_test: lonesha256_test
	./lonesha256_test

lonesha256_test: $(SRCDIR)/lonesha256/test.c $(SRCDIR)/lonesha256/lonesha256.h
	$(CC) $(CFLAGS) $(INCFLAGS) $(SRCDIR)/lonesha256/test.c -o lonesha256_test

clean:
	rm -f lonesha256_test
