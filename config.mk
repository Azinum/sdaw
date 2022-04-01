# config.mk

CC=gcc

PROG=sdaw

BUILD_DIR=build

RES=data

INC=include

USE_PORTAUDIO=1

LIB=-lpthread -lm -lpng -ldl

SRC=src/main.c

FLAGS=-I${INC} -Wall

O_RELEASE=-O3 -ffast-math

O_DEBUG=-O0 -g -W -Wall

DEBUG_PROG=

INSTALL_DIR=/usr/local/bin

LIB_PATH=/usr/local/lib

LIB_NAME=lib${PROG}

LIB_INC=/usr/local/include/${PROG}

VAR_DATA_PATH=/var/lib/${PROG}

DATA_PATH=/usr/local/lib/${PROG}
