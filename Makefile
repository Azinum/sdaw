# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}

compile: ${SRC}
	${CC} ${SRC} ${FLAGS} ${O_RELEASE}

install: compile
	chmod o+x ${BUILD_DIR}/${PROG}
	cp ${BUILD_DIR}/${PROG} ${INSTALL_DIR}/

uninstall:
	rm ${INSTALL_DIR}/${PROG}

clean:
	rm -r ${BUILD_DIR}/

debug: ${SRC}
	${CC} ${SRC} ${FLAGS} ${O_DEBUG}
	gdb ${BUILD_DIR}/${PROG}

run:
	./${BUILD_DIR}/${PROG} -g -f 12 -s 1 sb_nails.png
