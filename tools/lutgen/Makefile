# Makefile

all: prepare compile

prepare:
	mkdir -p build

compile:
	gcc lutgen.c -o build/lutgen -lm -O2 -Wall

run:
	./build/lutgen
