#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define SIGN(x) ((x) < 0 ? -1 : 1)

////////////////////////////////////////////////////////////////////////////////

uint16_t *heap_uint16_t(uint16_t);

int *heap_int(int);

void *mem_chk(void *);

void file_chk(int);

////////////////////////////////////////////////////////////////////////////////

#endif
