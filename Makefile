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

