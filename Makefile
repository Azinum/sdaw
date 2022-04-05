# Makefile

include config.mk
include platform.mk

all: compile

prepare:
	mkdir -p sequence
	mkdir -p output
	mkdir -p ${BUILD_DIR}
	cp -rp ${RES} ${BUILD_DIR}

compile: prepare ${SRC}
	${CC} ${SRC} -o ${BUILD_DIR}/${PROG} ${FLAGS} ${LIB} ${O_RELEASE}

install_mac: FLAGS += -D INSTALL_APPLE
install_mac: compile install
	./install_apple.sh

install:
	chmod o+x ${BUILD_DIR}/${PROG}
	cp ${BUILD_DIR}/${PROG} ${INSTALL_DIR}/
	mkdir -p ${DATA_PATH}
	cp -rp ${RES} ${DATA_PATH}/

uninstall:
	rm ${INSTALL_DIR}/${PROG}

clean:
	rm -r ${BUILD_DIR}/
	rm -r sequence/

debug:
	${CC} ${SRC} ${FLAGS} ${LIB} ${O_DEBUG}
	${DEBUG_PROG} ${BUILD_DIR}/${PROG}

shared: FLAGS+=-shared -fPIC -D NDEBUG
shared:
	${CC} ${SRC} -o ${BUILD_DIR}/${LIB_NAME}.so ${FLAGS} ${LIB} ${O_RELEASE}
	chmod o+x ${BUILD_DIR}/${LIB_NAME}.so
	cp ${BUILD_DIR}/${LIB_NAME}.so ${LIB_PATH}/
	mkdir -p ${LIB_INC}
	cp -R ${INC}/* ${LIB_INC}

run:
	./${BUILD_DIR}/${PROG}
