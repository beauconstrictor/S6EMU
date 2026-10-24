#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>

#define SERIAL 0xffff
#define HALT   0xfffe

#define HELP \
  "Usage: s6 [ROM]\n" \
  "A SUBLEQ-6 Emulator. Pass a ROM image up to 64K.\n" \
  "\n" \
  "Options:\n" \
  "  --version       Show version information and exit\n"

#define VERSION \
  "s6 (SUBLEQ-6 Emulator) v1.0.0\n" \
  "Copyright (c) 2026 Beau Constrictor\n" \
  "License GPLv2: GNU GPL version 2 <https://gnu.org/licenses/gpl.html>.\n" \
  "This is free software: you are free to change and redistribute it.\n" \
  "There is NO WARRANTY, to the extent permitted by law.\n"

typedef uint16_t word_t;

word_t ram[0x8000];
word_t rom[0x8000];

struct termios old_termios;

void initialise_terminal() {
  struct termios raw;

  tcgetattr(STDIN_FILENO, &old_termios);
  raw = old_termios;

  raw.c_lflag &= ~(ICANON | ECHO);
tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void restore_terminal() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);
}

char getkey(void) {
    char c;
    fd_set set;
    struct timeval timeout = {0, 0};

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    int ret = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);

    if (ret > 0) {
        if (read(STDIN_FILENO, &c, 1) == 1)
            return c;
    }

    return 0;
}

word_t fetch(word_t addr) {
  if (addr == SERIAL)
    return getkey();
  else if (addr < 0x8000)
    return ram[addr];
  else
    return rom[addr - 0x8000];
}

void put(word_t addr, word_t val) {
  if (addr == SERIAL)
    putchar(val & 0xff);
  else if (addr < 0x8000)
    ram[addr] = val;
}

word_t execute(word_t addr) {
  word_t a = fetch(addr++);
  word_t b = fetch(addr++);
  word_t c = fetch(addr++);

  word_t r = fetch(b) - fetch(a);
  put(b, r);

  if (r == 0 || (r & 0x8000))
    return c;
  else
    return addr;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("%s", HELP);
    return 1;
  };
  if (strcmp(argv[1], "--version") == 0) {
    printf("%s", VERSION);
    return 0;
  };

  char* path = argv[1];
  FILE *f = fopen(path, "rb");
  if (!f) {
    perror("fopen");
    return 1;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    perror("fseek");
    return 1;
  }
  long img_size = ftell(f);
  if (img_size < 0) {
    perror("ftell");
    return 1;
  } else if (img_size > 0x10000) {
    fprintf(stderr, "s6: rom image exceeds 64K");
    return 1;
  }
  rewind(f);

  uint8_t buf[0x10000];
  size_t read_size = fread(buf, 1, img_size, f);
  if (read_size != (size_t)img_size) {
    perror("fread");
    return 1;
  }
 
  fclose(f);

  for (size_t i = 0; i < read_size / 2; i++) {
    rom[i] = (word_t)buf[2 * i]
           | ((word_t)buf[2 * i + 1] << 8);
  }

  atexit(restore_terminal);
  initialise_terminal();

  word_t addr = 0x8000;
  while (addr != HALT)
    addr = execute(addr);

  return 0;
}
