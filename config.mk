# config.mk

CC=gcc

PROG=sdaw

BUILD_DIR=build

RES=resource

INC=include

# LIB_AUDIO=-lSDL2 -D USE_SDL
LIB_AUDIO=-lportaudio

LIB_COMMON=-lpthread -lm -lpng -lGLEW -lglfw ${LIB_AUDIO}

LIB_LINUX=${LIB_COMMON} -lGL -lGLU

LIB_MAC=${LIB_COMMON} -framework OpenGL

SRC=src/main.c

FLAGS=-I${INC} -Wall -Wno-missing-braces

O_RELEASE=-O3 -ffast-math

O_DEBUG=-O0 -g -W -Wall

INSTALL_DIR=/usr/local/bin

LIB_PATH=/usr/local/lib

LIB_NAME=lib${PROG}

LIB_INC=/usr/local/include/${PROG}

VAR_DATA_PATH=/var/lib/${PROG}

DATA_PATH=/usr/local/lib/${PROG}
