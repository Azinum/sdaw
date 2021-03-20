# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}

compile: ${SRC}
	${CC} ${SRC} ${FLAGS}

install: compile
	chmod o+x ${BUILD_DIR}/${PROG}
	cp ${BUILD_DIR}/${PROG} ${INSTALL_DIR}/

uninstall:
	rm ${INSTALL_DIR}/${PROG}

run:
	./${BUILD_DIR}/${PROG} -c 64 -w 1 -h 1 -x 1 -y 1 sb_nails.png
# aplay sb_nails.wav

