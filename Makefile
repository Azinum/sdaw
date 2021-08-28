# Makefile

include config.mk

all: prepare compile

prepare:
	mkdir -p sequence
	mkdir -p ${BUILD_DIR}
	cp -rp ${RES} ${BUILD_DIR}

compile: ${SRC}
	${CC} ${SRC} -o ${BUILD_DIR}/${PROG} ${FLAGS} ${LIB_LINUX} ${O_RELEASE}

mac: prepare compile_mac

compile_mac: ${SRC}
	${CC} ${SRC} -o ${BUILD_DIR}/${PROG} ${FLAGS} ${LIB_MAC} ${O_RELEASE}

install_mac: FLAGS += -D INSTALL_APPLE
install_mac: mac install
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

debug: ${SRC}
	${CC} ${SRC} ${FLAGS} ${LIB_LINUX} ${O_DEBUG}
	gdb ${BUILD_DIR}/${PROG}

shared_mac: FLAGS += -shared -fPIC -D NDEBUG
shared_mac:
	${CC} ${SRC} -o ${BUILD_DIR}/${LIB_NAME}.so ${FLAGS} ${LIB_MAC} ${O_RELEASE}
	chmod o+x ${BUILD_DIR}/${LIB_NAME}.so
	cp ${BUILD_DIR}/${LIB_NAME}.so ${LIB_PATH}/
	mkdir -p ${LIB_INC}
	cp -R ${INC}/* ${LIB_INC}

shared: FLAGS += -shared -fPIC -D NDEBUG
shared:
	${CC} ${SRC} -o ${BUILD_DIR}/${LIB_NAME}.so ${FLAGS} ${LIB_LINUX} ${O_RELEASE}
	chmod o+x ${BUILD_DIR}/${LIB_NAME}.so
	cp ${BUILD_DIR}/${LIB_NAME}.so ${LIB_PATH}/
	mkdir -p ${LIB_INC}
	cp -R ${INC}/* ${LIB_INC}

run:
	./${BUILD_DIR}/${PROG}
#	./${BUILD_DIR}/${PROG} -i resource/audio/dark_wind.ogg --width 512 --height 512 -v -s resource/plugins/modules/image_seq/test.mod -o sequence/frame_ --mask images/dark_wind.png
