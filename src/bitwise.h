#ifndef BIT_WISE_H
#define BIT_WISE_H

void    print_bits(int32_t);
int32_t bits_get(int32_t, int, int);
int32_t rotate_right(int32_t, int);

// Set/Clear/Get/Put the nth bit of integer i where 0 is rightmost bit

#define BIT_SET(i, n)       ((i) |=   1 << (n))
#define BIT_CLR(i, n)       ((i) &= ~(1 << (n)))
#define BIT_GET(i, n)    (!!((i) &   (1 << (n))))
#define BIT_PUT(i, n, b) { if (IS_SET(b)) BIT_SET(i, n); else BIT_CLR(i, n); }

#define TRUE  1
#define FALSE 0

#define IS_SET(i)   (i == 1)
#define IS_CLEAR(i) (i == 0)

#endif
