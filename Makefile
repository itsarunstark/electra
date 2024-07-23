CC=gcc
CFLAGS= -g -I./include -I.
LIB=./lib
BIN=./bin
SRC=./src
LINKS=-lwayland-client -lwayland-egl -lEGL -lGL -lGLEW -lm
default: create electra
	echo "build finished."

electra: ${LIB}/main.o ${LIB}/pointer.o ${LIB}/path.o ${LIB}/log.o ${LIB}/xdg-shell-client.o ${LIB}/xdg-client-output-z.o
	${CC} ${CFLAGS} $^ -o $@ ${LINKS}

${LIB}/main.o: ${SRC}/main.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/path.o: ${SRC}/path.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/pointer.o: ${SRC}/pointer.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/log.o: ${SRC}/log.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/xdg-shell-client.o: ${SRC}/xdg-shell-client.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/layer-shell-client-v1.o: ${SRC}/layer-shell-client-v1.c
	${CC} ${CFLAGS} -c -o $@ $^

${LIB}/xdg-client-output-z.o: ${SRC}/xdg-client-output-z.c
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
.PHONY: create clean memcheck

memcheck: electra
	valgrind --leak-check=full --gen-suppressions=all --log-fd=9 ./electra  9>>memcheck.log
