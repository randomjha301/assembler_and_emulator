CC     = gcc
CFLAGS = -std=c99 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes

all: asm emu

asm: asm.o parser.o symtab.o isa.o
	$(CC) $(CFLAGS) -o $@ $^

emu: emu.c
	$(CC) $(CFLAGS) -o $@ emu.c

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f asm.o parser.o symtab.o isa.o asm emu