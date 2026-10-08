CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
SRCS = ls.c cmp.c print.c util.c
OBJS = $(SRCS:.c=.o)
TARGET = ls

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c ls.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
