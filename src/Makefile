CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate 


assemble: assemble.o util/bitwise.o util/map.o util/tokens.o util/utils.o

emulate: emulate.o util/bitwise.o

clean:
	rm -f $(wildcard *.o)
	rm -f util/*.o
	rm -f assemble
	rm -f emulate
