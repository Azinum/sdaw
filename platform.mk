# platform.mk

MACHINE=${strip ${shell ${CC} -dumpmachine}}
ifndef PLATFORM
	PLATFORM=UNKNOWN
	ifneq (, ${findstring -linux, ${MACHINE}})
		PLATFORM=LINUX
	endif
	ifneq (, ${findstring -freebsd, ${MACHINE}})
		PLATFORM=BSD
	endif
	ifneq (, ${findstring -mingw32, ${MACHINE}})
		PLATFORM=WINDOWS
	endif
	ifneq (, ${findstring -darwin, ${MACHINE}})
		PLATFORM=DARWIN
	endif
endif

ifeq (${USE_PORTAUDIO}, 1)
	LIB+=-lportaudio
else ifeq (${USE_SDL}, 1)
	LIB+=-lSDL2
	FLAGS+=-D USE_SDL
endif

ifeq (${PLATFORM}, LINUX)
	LIB+=-lglfw -lGLEW -lGL -lGLU
	DEBUG_PROG=gdb
endif

ifeq (${PLATFORM}, WINDOWS)

endif

ifeq (${PLATFORM}, DARWIN)
	LIB+=-lglfw -lGLEW -framework OpenGL -framework CoreMIDI -framework CoreFoundation
	FLAGS+=-Wno-missing-braces
	DEBUG_PROG=lldb
endif

