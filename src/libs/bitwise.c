#include <stdio.h>
#include <stdint.h>
#include "bitwise.h"




int32_t bits_get(int32_t i, int n, int m)
{
	// (m > n) making the mask unsigned solves the problem... but y?
	if (m - n == 31) return i;
	uint32_t mask = ((1 << (m - n + 1)) - 1) << n;
	return (i & mask) >> n;
}


// Prints the bits of integer i to the standard output
void print_bits(int32_t i)
{
	// printf("Integer %i in binary : ", i);
	for (int c = 31; c >= 0; c--)
	{
		char bit = ((i >> c) & 1) ? '1' : '0';
		printf("%c", bit);
		if (c % 8 == 0) printf(" ");
	}
	printf("\n");
}


// Prints the bits of integer i to the standard output in BIG ENDIAN
void print_bits_BE(int32_t i)
{
	// VERYUGLY BUT WORKS
	
	//print_bits(((i & 0xFF) << 24) | ((i & 0xFF00) << 8) | ((i >> 8) & 0xFF00) | (i >> 24));
	
	print_bits(((i>>24)&0xff) | // move byte 3 to byte 0
	((i<<8)&0xff0000) | // move byte 1 to byte 2
	((i>>8)&0xff00) | // move byte 2 to byte 1
						 ((i<<24)&0xff000000)); // byte 0 to byte 3
	 
//
//	int _1 = (i & 0xFF) >> 24;
//	int _2 = (i & 0xFF) >> 16;
//	int _3 = (i & 0xFF) >>  8;
//	int _4 = (i & 0xFF) >>  0;
//	
//	print_bits(_1 | _2 | _3 | _4);
}


// Rotates bits to right
int32_t rotate_right(int32_t i, int n)
{
  for (int j = 0; j < n; j++)
  {
    int last_bit = BIT_GET(i, 0);
    i >>= 1;
    BIT_PUT(i, 31, last_bit); // i = fucck
  }
  return i;
}














