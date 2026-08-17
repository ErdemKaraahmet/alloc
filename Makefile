CC := gcc
CFLAGS := -std=gnu11 -Wall -Wextra -Wpedantic
HEADERS := alloc.h
LIB_SRCS := alloc.c
LIB_OBJS := $(LIB_SRCS:.c=.o)
TEST_SRC := tests/test_alloc.c
TEST_TARGET := test_runner

.PHONY: all test debug clean format 

all: $(LIB_OBJS)

test: $(LIB_OBJS) $(TEST_SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(TEST_SRC) -o $(TEST_TARGET)
	./$(TEST_TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g3 -O0
debug: clean test

clean: 
	rm -rf $(LIB_OBJS) $(TEST_TARGET) tests/*.o

format:
	clang-format -i *.c *.h tests/*.c