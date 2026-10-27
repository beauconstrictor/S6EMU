# Ozpex S6

The Ozpex S6 is the fourth fantasy computer in my Ozpex family. This one breaks the cycle is ever-increasing power and complexity of the previous 3 and is designed to be the simplest possible computer to design and build, while still maintaining the capacity for universal computation.

The CPU is based on the [SUBLEQ](https://esolangs.org/wiki/Subleq) architecture, which features a single instruction. This makes implementation simpler by eliminating the need for address decoding entirely. The downside is that the instruction itself is quite complex (and consumes a lot of bytes, making programs very large) and a lot of them are needed to accomplish fairly trivial tasks. Effectively, `subleq` moves the inherent complexity of computing from the hardware to the software. Luckily for me, software is my speciality, so this is an excellent tradeoff.

There are currently two parts to this project:

- [S6ASM](https://github.com/beauconstrictor/s6asm) - A macro assembler (and integrated emulator) for the S6 assembly language.
- [S6EMU](https://github.com/beauconstrictor/s6emu) (this repo) - A standalone emulator for the S6, written in C.

The computer's name, the S6, comes from comes from it's characteristic 6-byte `subleq` instructions, making the 'SUBLEQ-6', which is then abbreviated to just S6.

## This Emulator

This repository contains a sample emulator for the system, written in C. To download & build it, install `clang` and `make` and run:

```
$ git clone https://github.com/beauconstrictor/s6emu
$ make
```

Then, to run a ROM image (produced by [S6ASM](https://github.com/beauconstrictor):

```
./build/s6emu rom-image.bin
```

## Characteristics

The system is 8-bit (using two's complement arithmetic), with 16-bit addressing. Instructions are 6-byte, fixed width.

### The Instruction

The instruction is called `subleq a b c`. Since this is the _only_ instruction in the system, it's opcode is implied, leaving only the 3 unsigned 16-bit operands.

This is what the instruction does:

1. Read from memory address *A*
2. Subtract that from what's at memory address *B* and store it back into *B*.
3. If the result (interpreted using two's complement) was less than or equal to zero, branch to *C*. Otherwise, continue to the next instruction.

As you can see, this instruction encodes 4 useful properties at once (addition, negation, conditional testing and jumping). Much of S6 programming is about teasing the individual operations apart.

For example, since negation in two's complement is a bitwise NOT and an increment, you can implement a NOT by negating and then decrementing. An unconditional loop comes from ensuring the condition is always true (setting A and B to the same address)

### The Memory Map

The Ozpex S6's memory map is incredibly simple:

- The bottom 32K of address space is general RAM
- The top 32K of address space is general ROM, *except*:
- Writes to `0xFFFE` output to the serial UART
- Reads from `0xFFFF` return the last character recieved from the UART. Subsequent reads will return a NULL, until another byte is recieved

Also, if the program counter ever reaches `0xFFFE`, then the machine simply shuts down. The program counter starts at `0x8000` when the machine powers on.
