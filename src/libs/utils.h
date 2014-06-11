#include <stdint.h>
#ifndef UTILS_H
#define UTILS_H

////////////////////////////////////////////////////////////////////////////////

#define HEAP_PRIM(type, value)\
	do\
	{\
		type *pt = malloc(sizeof(type));\
		*pt = value;\
		return pt;\
	} while(0)

uint16_t *heap_uint16_t(uint16_t);

int *heap_int(int);

////////////////////////////////////////////////////////////////////////////////

#endif
