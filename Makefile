# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}
	mkdir -p sequence

compile: ${SRC}
	${CC} ${SRC} ${FLAGS} ${LIB} ${O_RELEASE}

mac: prepare compile_mac run

compile_mac: ${SRC}
	${CC} ${SRC} ${FLAGS} ${LIB_MAC} ${O_RELEASE}

install: compile
	chmod o+x ${BUILD_DIR}/${PROG}
	cp ${BUILD_DIR}/${PROG} ${INSTALL_DIR}/

uninstall:
	rm ${INSTALL_DIR}/${PROG}

clean:
	rm -r ${BUILD_DIR}/
	rm -r sequence/

debug: ${SRC}
	${CC} ${SRC} ${FLAGS} ${LIB} ${O_DEBUG}
	gdb ${BUILD_DIR}/${PROG}

run:
	./${BUILD_DIR}/${PROG} -I -f images/a.png -t images/b.png -o images/c.png
#	./${BUILD_DIR}/${PROG} -i audio/dark_wind.wav -r 24 -W 1024 -H 1024 -S -o sequence/frame_ -v -s 2 --mask images/dark_wind.png --start-index 250 --num-frames 12
