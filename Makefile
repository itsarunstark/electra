CC=gcc
CFLAGS= -g -I./include
LIB=./lib
BIN=./bin
SRC=./src
LINKS=-lwayland-client
default: create electra
	echo "build finished."

electra: ${LIB}/main.o ${LIB}/path.o ${LIB}/log.o
	${CC} ${CFLAGS} $^ -o $@ ${LINKS}

${LIB}/main.o: ${SRC}/main.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/path.o: ${SRC}/path.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/log.o: ${SRC}/log.c
	${CC} ${CFLAGS} -c -o $@ $^
create:
	mkdir -p lib bin logs
	echo "created paths"
clean:
	rm -rf ./lib/*
	rm -rf ./bin/*
	rm -rf ./logs/*
	rm -rf ./bin
	rm -rf ./lib
	rm -rf ./logs
.PHONY: create clean

