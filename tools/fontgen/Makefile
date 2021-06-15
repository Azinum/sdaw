# Makefile - fontgen

CC=gcc

FLAGS=-lSDL2 -lSDL2_ttf

all: compile

compile:
	${CC} fontgen.c ${FLAGS} -o fontgen

run:
	./fontgen font.ttf asciilist_utf8.txt font.bmp
