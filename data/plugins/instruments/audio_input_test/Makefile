# Makefile

CC=gcc

C_FLAGS=-I/usr/local/include/sdaw -lsdaw -O2

SRC=audio_input_test

EXT=ins

all: ${SRC}

${SRC}:
	${CC} ${C_FLAGS} $@.c -o $@.${EXT} -Wall

.PHONY: ${SRC}
