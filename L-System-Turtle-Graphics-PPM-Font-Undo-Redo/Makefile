CC=gcc
CFLAGS=-Wall -Wextra -std=c99

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)


TARGETS=runic

build: $(OBJS) $(TARGETS)

runic: runic.c
		$(CC) $(CFLAGS) $@.o -o $@ -lm

pack:
		zip -FSr 311CB_SavlovschiAndrei_Tema3.zip README Makefile *.c *.h


clean:
		rm -f $(TARGETS) $(OBJS) 311CB_SavlovschiAndrei_Tema3.zip
