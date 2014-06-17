#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "instruction.h"

////  GET ENUM FROM STRING FUNCTION  ///////////////////////////////////////////

#define STR_TO_ENUM(a) int str_to_##a(char *x)           \
{                                                        \
	int len = sizeof(a##_array)/sizeof(a##_array[0]);      \
	for (int i = 0; i < len; i++)                          \
	{	                                                     \
		char *low_x = strtolwr(a##_array[i].str);        \
		if (strcmp(x, low_x) == 0)                           \
		{                                                    \
			return a##_array[i].no;                            \
		}                                                    \
	}                                                      \
	return -1;                                             \
}


STR_TO_ENUM(mnemonic)
STR_TO_ENUM(opcode)
STR_TO_ENUM(cond)
STR_TO_ENUM(shift)


