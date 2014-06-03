#ifndef _SYMBOL_TABLE_INTERFACE
#define _SYMBOL_TABLE_INTERFACE

#include <stdint.h>

#include "symtable_T.h"

// i know this is NOT an ADT but give me a break. 
// no templates, only void pointers? no way
// fuck off, c.

// initiates and allocates memory for a symbol table
SymbolTable *symtable_init();

// clears the table and frees all memory
void symtable_clear(SymbolTable *table);

// adds an entry to a table
void symtable_put(SymbolTable *table, const char *key, uint32_t value);

// removes an entry from a table (if its key exists)
void symtable_remove(SymbolTable *table, const char *key);

// returns an entry from a table (if its key exists)
uint32_t symtable_get(SymbolTable *table, const char *key);

// checks if a key is in a table
int symtable_contains(SymbolTable *table, const char *key);

// prints the table and its contents
void symtable_print(SymbolTable *table);


#endif
