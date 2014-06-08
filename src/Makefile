CC      = gcc
CFLAGS  = -Wall -g -D_BSD_SOURCE -D_POSIX -std=c99 -Werror

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate 


assemble: assemble.o libs/tokens.o libs/map.o libs/bitwise.o libs/assembler.o  libs/utils.o

emulate: emulate.o libs/bitwise.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
	rm -f tokens 
	rm -f libs/map 
	rm -f libs/bitwise 
	rm -f libs/assembler
