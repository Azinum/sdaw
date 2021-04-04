# config.mk

CC=gcc

PROG=sdaw

BUILD_DIR=build

INC=include

LIB=-lpng -lportaudio -lm

SRC=src/main.c

FLAGS=-o ${BUILD_DIR}/${PROG} ${LIB} -I${INC} -Wall

O_RELEASE=-O2

O_DEBUG=-O0 -g -W

INSTALL_DIR=/usr/local/bin
