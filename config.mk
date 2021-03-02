# config.mk

CC=gcc

PROG=sdaw

BUILD_DIR=build

INC=include

LIB=-lpng -lportaudio

SRC=src/main.c

FLAGS=-o ${BUILD_DIR}/${PROG} ${LIB} -I${INC} -O2 -Wall

