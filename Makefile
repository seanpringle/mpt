
OBJECTS=$(shell ls -1 src/*.c | sed 's/c$$/o/g')

dev: CFLAGS=-O3 -flto -std=c11 -g -Wall -Werror $(shell pkg-config --cflags cairo)
dev: LFLAGS=-lm $(shell pkg-config --libs cairo) -pthread
dev: $(OBJECTS)
	$(CC) $(CFLAGS) -o mpt $(OBJECTS) $(LFLAGS)

clean:
	rm -f src/*.o mpt

compat:
	$(MAKE) clean
	CC=gcc $(MAKE) dev
	$(MAKE) clean
	CC=clang $(MAKE) dev
	$(MAKE) clean

src/%.o: src/%.c src/%.h src/common.h
	$(CC) $(CFLAGS) -c $< -o $@
