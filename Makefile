# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}
	mkdir -p sequence
	cp -rp ${RES} ${BUILD_DIR}

compile: ${SRC}
	${CC} ${SRC} ${FLAGS} ${LIB_LINUX} ${O_RELEASE}

mac: prepare compile_mac run

compile_mac: ${SRC}
	${CC} ${SRC} ${FLAGS} ${LIB_MAC} ${O_RELEASE}

install:
	chmod o+x ${BUILD_DIR}/${PROG}
	cp ${BUILD_DIR}/${PROG} ${INSTALL_DIR}/

uninstall:
	rm ${INSTALL_DIR}/${PROG}

clean:
	rm -r ${BUILD_DIR}/
	rm -r sequence/

debug: ${SRC}
	${CC} ${SRC} ${FLAGS} ${LIB_LINUX} ${O_DEBUG}
	gdb ${BUILD_DIR}/${PROG}

run:
	./${BUILD_DIR}/${PROG}
