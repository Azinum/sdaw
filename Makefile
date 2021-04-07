# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}
	mkdir -p sequence

compile: ${SRC}
	${CC} ${SRC} ${FLAGS} ${O_RELEASE}

install: compile
	chmod o+x ${BUILD_DIR}/${PROG}
	cp ${BUILD_DIR}/${PROG} ${INSTALL_DIR}/

uninstall:
	rm ${INSTALL_DIR}/${PROG}

clean:
	rm -r ${BUILD_DIR}/
	rm -r sequence/

debug: ${SRC}
	${CC} ${SRC} ${FLAGS} ${O_DEBUG}
	gdb ${BUILD_DIR}/${PROG}

run:
	./${BUILD_DIR}/${PROG} -i concrete_gloom_low.wav -r 24 -W 512 -H 512 -S -o sequence/frame_ -v -s 2 -i 0 -n 250
