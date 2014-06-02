#ifndef _SYMBOL_TABLE_DATATYPE
#define _SYMBOL_TABLE_DATATYPE

#include <stdint.h>

#define TABLE_SIZE 203

// one entry in the symbol table, with a char array key
// representing the label and an unsigned integer value 
// representing the address
typedef struct TableEntry 
{
	const char *key;
	uint32_t  value;
	int        flag;
} SymbolTable[TABLE_SIZE];


#endif
