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

// 0 extend to 32-bit
#define ZERO_EXT_32(i) ((i) & 255)

////////////////////////////////////////////////////////////////////////////////
//  FUNCTIONS  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Puts bit b in int i at position n
int32_t bit_put(int32_t i, int n, int b)
{
  if (b==1) BIT_SET(i,n); 
  else BIT_CLEAR(i,n);
  return i;
}

// Gets bits n to m of integer i where 0 is the rightmost bit
int32_t bits_get(int32_t i, int n, int m)
{
	return (i >> n) & ((1 << m) - 1);
}

// Sets bits n to m of integer i to the value of bits n to m of integer j
int32_t bits_set(int32_t i, int32_t j, int n, int m)
{	
	int32_t seq = bits_get(j, n, m);
	return (i & ~(((1 << (m - n + 1)) - 1) << n)) | (seq << n);
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
  for(int j=0; j<n; j++)
  {
    int last_bit = BIT_GET(i,0);
    i >>= 1;
    bit_put(i, 31, last_bit);
  }
  return i;
}














