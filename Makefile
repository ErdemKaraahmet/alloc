CC := gcc
CFLAGS := -std=gnu11 -Wall -Wextra -Wpedantic
HEADERS := alloc.h
SRCS := main.c alloc.c
OBJS := $(SRCS:.c=.o)
TARGET := program

.PHONY: all clean format

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -rf $(OBJS) $(TARGET)

format:
	clang-format -i *.c *.h