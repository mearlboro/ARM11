#ifndef _BIT_WISE
#define _BIT_WISE


   void print_bits(int32_t i);
int32_t   bits_get(int32_t i, int n, int m);
int32_t  bits_get2(int32_t i, int n, int m);
int32_t     rotate(int32_t i, int n);
int32_t    bit_put(int32_t i, int n, int b);


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

#define BITS_GET(n, i, j)  ((((1 << (j)) - (1 << (i))) & (n)) >> (i))

#define IS_SET(i) (i == 1)

#define IS_CLEAR(i) (i == 0)

#endif










