# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}
	mkdir -p sequence
	cp -rp ${RES} ${BUILD_DIR}

compile: ${SRC}
	${CC} ${SRC} -o ${BUILD_DIR}/${PROG} ${FLAGS} ${LIB_LINUX} ${O_RELEASE}

mac: prepare compile_mac run

compile_mac: ${SRC}
	${CC} ${SRC} -o ${BUILD_DIR}/${PROG} ${FLAGS} ${LIB_MAC} ${O_RELEASE}

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

shared_mac:
	${CC} ${SRC} -o ${BUILD_DIR}/${LIB_NAME}.so ${FLAGS} ${LIB_MAC} ${O_RELEASE} -shared -fPIC -D NDEBUG
	chmod o+x ${BUILD_DIR}/${LIB_NAME}.so
	cp ${BUILD_DIR}/${LIB_NAME}.so ${LIB_PATH}/
	mkdir -p ${LIB_INC}
	cp -R ${INC}/* ${LIB_INC}

shared:
	${CC} ${SRC} ${FLAGS} ${LIB_LINUX} ${O_RELEASE}
	chmod o+x ${BUILD_DIR}/${LIB_NAME}.so
	cp ${BUILD_DIR}/${LIB_NAME}.so ${LIB_PATH}/
	mkdir -p ${LIB_INC}
	cp -R ${INC}/* ${LIB_INC}

run:
	./${BUILD_DIR}/${PROG}
