
CC=gcc
CFLAGS=-O3 -flto -march=native -std=c11 -g -Wall -Werror $(shell pkg-config --cflags cairo luajit)
LFLAGS=-lm $(shell pkg-config --libs cairo luajit) -pthread
OBJECTS=$(shell ls -1 src/*.c | sed 's/c$$/o/g')

build: $(OBJECTS)
	$(CC) $(CFLAGS) -o mpt $(OBJECTS) $(LFLAGS)

clean:
	rm -f src/*.o src/*gcda mpt

compat:
	$(MAKE) clean
	CC=gcc $(MAKE) build
	$(MAKE) clean
	CC=clang $(MAKE) build
	$(MAKE) clean

lua:
	xxd -i src/lib.lua src/lua-lib.h

src/%.o: src/%.c src/%.h src/common.h
	$(CC) $(CFLAGS) -c $< -o $@
