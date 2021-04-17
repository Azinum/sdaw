# Makefile

all: prepare compile

prepare:
	mkdir -p build

compile:
	gcc main.c -o build/freq -lm -O2 -Wall

run:
	./build/freq > freq_table.c
