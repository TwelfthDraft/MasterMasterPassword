CC=gcc
CFLAGS=
INCFLAGS=-Isrc -Isrc/lonesha256/

OBJDIR=obj
SRCDIR=src

MAIN_HEAD=

MAIN_SRC=

TEST_SRC  := $(wildcard $(SRCDIR)/*test*.c)
TEST_HEAD := $(wildcard $(SRCDIR)/*test*.h)

BOTH_SRC  := $(wildcard $(SRCDIR)/*.c)
BOTH_SRC  := $(filter-out $(MAIN_SRC), $(BOTH_SRC))
BOTH_SRC  := $(filter-out $(TEST_SRC), $(BOTH_SRC))

BOTH_HEAD := $(wildcard $(SRCDIR)/*.h)
BOTH_HEAD := $(filter-out $(MAIN_HEAD), $(BOTH_HEAD))
BOTH_HEAD := $(filter-out $(TEST_HEAD), $(BOTH_HEAD))

LONE_TEST := $(SRCDIR)/lonesha256/test.c
LONE_HEAD := $(SRCDIR)/lonesha256/lonesha256.h

all: test
	echo "Main src:  " $(MAIN_SRC)
	echo "Main head: " $(MAIN_HEAD)
	echo "Test src:  " $(TEST_SRC)
	echo "Test head: " $(TEST_HEAD)
	echo "Both src:  " $(BOTH_SRC)
	echo "Both head: " $(BOTH_HEAD)


.PHONY: test
test: tb_test
	./tb_test

tb_test: $(TEST_HEAD) $(BOTH_HEAD) $(TEST_SRC) $(BOTH_SRC) $(LONE_HEAD)
	$(CC) $(CFLAGS) $(INCFLAGS) $(TEST_SRC) $(BOTH_SRC) -o tb_test

.PHONY: sha_test
sha_test: lonesha256_test
	./lonesha256_test

lonesha256_test: $(LONE_TEST) $(LONE_HEAD)
	$(CC) $(CFLAGS) $(INCFLAGS) $(LONE_TEST) -o lonesha256_test

clean:
	rm -f lonesha256_test
	rm -f tb_test
