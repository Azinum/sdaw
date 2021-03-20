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
	./${BUILD_DIR}/${PROG} -c 16 -w 0.5 -h 0.5 -x 12 -y 4 sb_nails.png
# aplay sb_nails.wav

