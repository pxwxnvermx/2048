CC = clang++
CFLAGS= -std=c++17 -Wall -Wextra -ggdb 
LIBS= -lraylib -lm -ldl -lpthread -lglfw -lc
CFILES = main.cpp

clean:
	rm ./bin/main

build: 
	${CC} ${CFLAGS} -o ./bin/main ${CFILES} ${LIBS}

run: build
	./bin/main

all: clean build

RAYLIB_PATH = ${HOME}/Repos/software/raylib/src
WEBCC = em++
WEBCFLAGS = -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I ${RAYLIB_PATH} -I ${RAYLIB_PATH}/external -L. -L ${RAYLIB_PATH} -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 ${RAYLIB_PATH}/web/libraylib.a -DPLATFORM_WEB 

build-web:
	${WEBCC} -o ./bin/game.js ${CFILES} ${WEBCFLAGS}
