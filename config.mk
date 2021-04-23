# config.mk

CC=gcc

PROG=sdaw

BUILD_DIR=build

INC=include

LIB=-lpng -lportaudio -lm -lGLEW -lglfw -lGL -lGLU

LIB_MAC=-lpng -lportaudio -lm -lGLEW -lglfw -framework OpenGL

SRC=src/main.c

FLAGS=-o ${BUILD_DIR}/${PROG} -I${INC} -Wall

O_RELEASE=-O2 -ffast-math

O_DEBUG=-O0 -g -W -Wall

INSTALL_DIR=/usr/local/bin
