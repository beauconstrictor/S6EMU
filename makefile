CC = clang
CFLAGS = -Wall -Wextra -g -Wno-unused-parameter

.PHONY: all 
all: build/s6

build/:
	mkdir -p build

build/s6: src/main.c | build/
	$(CC) $(CFLAGS) $< -o $@

.PHONY: run 
run: build/s6
	./build/s6

.PHONY: dbg 
dbg: build/s6
	gdb ./build/s6

clean:
	rm -rf build
