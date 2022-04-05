# Makefile

CC=gcc

C_FLAGS=-I/usr/local/include/sdaw -lsdaw -O3 -shared -fPIC

SRC=${subst .c,, ${wildcard *.c}}

all: test

${SRC}:
	${CC} $@.c -o $@.mod ${C_FLAGS} -Wall

clean:
	rm *.mod

.PHONY: test
