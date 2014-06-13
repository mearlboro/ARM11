#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////

uint16_t *heap_uint16_t(uint16_t i)
{
	uint16_t *pt = malloc(sizeof(uint16_t));
	*pt = i;
	return pt;
}

int *heap_int(int i)
{
	int	*pt = malloc(sizeof(int));
	*pt = i;
	return pt;
}

void *mem_chk(void *p)
{
	if (p == NULL)
	{
		perror("INSUFFICIENT MEMORY");
		exit(EXIT_FAILURE);
	}
	return p;
}

void file_chk(int predicate)
{
	if (!predicate)
	{
		perror("FILE ERROR");
		exit(EXIT_FAILURE);
	}
}

////////////////////////////////////////////////////////////////////////////////
