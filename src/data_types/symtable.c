#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "symtable_I.h"

#define PUT(i,a,b,c) table[i]->key = a; table[i]->value = b; table[i]->flag = c


// generates a hash value for each label
int hash_function(const char *key) 
{
	int hashvalue = 0;

	for (int i=0; i<strlen(key); i++)
	{
		hashvalue += (key[i] - '0') * i;
	}

	return hashvalue % TABLE_SIZE;
}


SymbolTable *symtable_init() 
{
	SymbolTable *table = calloc(TABLE_SIZE, sizeof(struct TableEntry));

	return table;
} 


void symtable_clear(SymbolTable *table) 
{
	for (int i=0; i<TABLE_SIZE; i++) free(table + i);
}


void symtable_put(SymbolTable *table, const char *key, uint32_t value)
{
	int hashvalue = hash_function(key);

	if (table[hashvalue]->flag == 0) 
	{
		PUT(hashvalue, key, value, 1);
	}
	else 
	{
	// TODO: double hash
	}
}


void symtable_remove(SymbolTable *table, const char *key) 
{
	assert(symtable_contains(table, key) == 1);

	PUT(hash_function(key), NULL, 0, 0);
}


uint32_t symtable_get(SymbolTable *table, const char *key)
{
	assert(symtable_contains(table, key) == 1);

	return table[hash_function(key)]->value;
}

	
int symtable_contains(SymbolTable *table, const char *key) 
{
	return (table[hash_function(key)]->flag == 1);
}


void symtable_print(SymbolTable *table) 
{
	printf("Printing symbol table \n");

	for(int i=0; i<TABLE_SIZE; i++) 
	{
		printf("\t(%s, %ul)\n", table[i]->key, table[i]->value);
	}
}



