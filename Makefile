# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}

compile: ${SRC}
	${CC} ${SRC} ${FLAGS}

run:
	./${BUILD_DIR}/${PROG}

