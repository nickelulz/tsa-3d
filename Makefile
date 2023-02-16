# Global

SRC := $(wildcard src/*.c) \
	   $(wildcard src/**/*.c) \
	   $(wildcard src/**/**/*.c) \
	   $(wildcard src/**/**/**/*.c)

OUT := game

# Default Build

EXE := ./build/${OUT}
CFLAGS := -I./src/include
LIBFLAGS := -lSDL2 -ldl
OBJ := ${SRC:.c=.o}

build: clean-obj dir ${OBJ}
	gcc ${OBJ} -o ${EXE} ${CFLAGS} ${LIBFLAGS}
	${call clean-obj}

%.o: %.c
	gcc -c $< -o $@ ${CFLAGS}

run: build
	./bin/game

# Web Build

WEBEXE := ./build/${OUT}.js
EMFLAGS = -s USE_SDL=2 -s WASM=1
EMFLAGS += -03
WEB_OBJ := ${SRC:.c=.wo}

html: clean-obj dir ${WEB_OBJ}
	emcc ${WEB_OBJ} -o ${WEBEXE} ${CFLAGS} ${EMFLAGS}
	${call clean-obj}

%.wo: %.c
	emcc -c $< -o $@ ${CFLAGS}

html-run: html
	emrun ./bin/web/test.html

# Utility

dir:
	mkdir -p ./bin ./build/

clean-obj:
	rm -f ${OBJ} ./bin/*

clean: clean-obj
	rm -f ${WEBEXE} ${EXE} ./build/${OUT}.wasm