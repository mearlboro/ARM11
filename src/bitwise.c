#include <stdio.h>
#include <stdint.h>
#include "bitwise.h"


int32_t bit_put(int32_t i, int n, int b)
{
    if (b == 1) BIT_SET(i, n);
    else BIT_CLEAR(i, n);
    return i;
}


// Gets bits n to m of integer i where 0 is the rightmost bit
int32_t bits_get(int32_t i, int n, int m)
{
    int32_t mask = 0;
    for (int j = n ; j <= m + 1 ; j++)
    {
        int32_t x = 1 << j;
        mask += x;
    }
    
    i = i & mask;
    i = i >> n;
    return i;
}


int32_t bits_get2(int32_t i, int n, int m)
{
	// (m > n) making the mask unsigned solves the problem... but y?
	if (m - n == sizeof(i) * 8 - 1) return i;
	uint32_t mask = ((1 << (m - n + 1)) - 1) << n;
	return (i & mask) >> n;
}


// Prints the bits of integer i to the standard output
void print_bits(int32_t i)
{
	// printf("Integer %i in binary : ", i);
	for (int c = sizeof(i) * 8 - 1; c >= 0; c--)
	{
		char bit = ((i >> c) & 1) ? '1' : '0';
		printf("%c", bit);
		if (c % 8 == 0) printf(" ");
	}
	printf("\n");
}


// Rotates bits to right
int32_t rotate(int32_t i, int n)
{
    for (int j = 0; j < n; j++)
    {
        int last_bit = BIT_GET(i, 0);
        i >>= 1;
        i = bit_put(i, 31, last_bit); // i = fucck
    }
    return i;
}














