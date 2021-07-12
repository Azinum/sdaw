# Makefile

CC=gcc

C_FLAGS=-I/usr/local/include/sdaw -lsdaw -O3 -shared -fPIC

SRC=${wildcard *.c}

PROG=test

all: compile

compile:
	${CC} test.c -o ${PROG}.mod ${C_FLAGS} -Wall

clean:
	rm *.mod
