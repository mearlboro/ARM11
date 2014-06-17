#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

////  1. MEMORY UTILS //////////////////////////////////////////////////////////

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


////  2. STRING UTILS  /////////////////////////////////////////////////////////

/*
 * Deletes the character chr from the string buffer
 * returning the new string 
 */
char *strdelchr(char *buffer, char chr)
{
    char *str = strdup(buffer);
    char* c;

	if ((c = index(str, chr)) != NULL)
	{
	    size_t left = sizeof(str) - (c + 1 - str);
	    memmove(c, c + 1, left);
	}

	return str;
}

