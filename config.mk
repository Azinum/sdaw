# config.mk

CC=gcc

PROG=sdaw

BUILD_DIR=build

RES=resource

INC=include

LIB_COMMON=-lpthread -lm -lpng -lportaudio -lGLEW -lglfw

LIB_LINUX=${LIB_COMMON} -lGL -lGLU

LIB_MAC=${LIB_COMMON} -framework OpenGL

SRC=src/main.c

FLAGS=-o ${BUILD_DIR}/${PROG} -I${INC} -Wall

O_RELEASE=-O2 -ffast-math

O_DEBUG=-O0 -g -W -Wall

INSTALL_DIR=/usr/local/bin
