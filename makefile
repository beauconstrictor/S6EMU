CC = clang
CFLAGS = -Wall -Wextra -g -Wno-unused-parameter

.PHONY: all 
all: build/s6emu

build/:
	mkdir -p build

build/s6emu: src/main.c | build/
	$(CC) $(CFLAGS) $< -o $@

.PHONY: run 
run: build/s6emu
	./build/s6emu

.PHONY: dbg 
dbg: build/s6emu
	gdb ./build/s6emu

clean:
	rm -rf build
