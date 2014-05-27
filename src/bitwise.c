#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//  FUNCTION PROTOTYPES  ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void print_bits(int32_t i);
char *int_to_bits_array(int32_t i);
int32_t bits_array_to_int(char *bits_array);



////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS AND MACROS  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Sets the nth bit of integer i where 0 is rightmost bit 
#define BIT_SET(i, n) ((i) |= 1 << (n)) 

// Clears the nth bit of integer i where 0 is rightmost bit 
#define BIT_CLEAR(i, n) ((i) &= ~(1 << (n))) 

// Toggles the nth bit of integer i where 0 is rightmost bit 
#define BIT_TOGGLE(i, n) ((i) ^= 1 << (n)) 

// Gets the nth bit of integer i where 0 is the rightmost bit 
#define BIT_GET(i, n) (!!((i) & (1 << (n))))

// Gets bits n to m of integer i where 0 is the rightmost bit
int32_t bits_get(int32_t i, int n, int m)
{
  int32_t mask = 0;
  mask = ((1 << m) - 1) - ((1 << n) - 1) ;
  i = i & mask;
  i = i >> n;
  return i;
}

// Sets bits n to m of integer i to the value of bits n to m of integer j
#define BITS_SET(i, j, n, m) (0) // TODO



////////////////////////////////////////////////////////////////////////////////
//  CORE  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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

// Returns the binary string representation of integer i
char *int_to_bits_array(int32_t i) 
{
	return NULL;
}

// Returns the integer value of the binary string representation bits_array
int32_t bits_array_to_int(char *bits_array) 
{
	return 0;
}














