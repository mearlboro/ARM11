#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "tokens.h"
#include "map.h"
#include "utils.h"
#include "assembler.h"


//////////////////////////////////////////////////////  Free An Assembly Program

static void ass_prog_free(ass_prog *p)
{
	int i = p->line_tot;
	while (i--) free(p->instrs[i]);
	free(p->instrs);
	map_free(p->sym_tbl, MAP_FREE_VAL);
	free(p);
}

///////////////////////////////////////////////  Extend Binary Instruction Array

static bin_instr **realloc_instrs(ass_prog *p)
{
	return realloc(p->instrs, sizeof(bin_instr *) * (p->line_tot));
}

//////////////////////////////////////////////////////  Generate ARM-Binary Code

static int32_t *ass_prog_gen(ass_prog *p)
{
	int32_t *words = malloc(sizeof(int32_t) * p->line_tot);
	for (int i = 0; i < p->line_tot; i++)
	{
		words[i] = p->instrs[i]->bin_word;
	}
	return words;
}

////////////////////////////////////////////////////////////  Insert Binary Word

uint16_t ass_prog_insert(ass_prog *p, int32_t word, int line)
{
	// Appending to end of program
	if (line == p->line_tot)
	{
		p->line_tot++;
		line++;
		realloc_instrs(p);
	}
	
	uint16_t address = line * sizeof(int32_t);
	bin_instr *instr = malloc(sizeof(bin_instr));
	instr->bin_word  = word;
	instr->word_addr = address;
	assert(address == p->curr_addr);
	
	p->instrs[line]  = instr;

	return address;
}

///////////////////////////////////////////////////////  Assemble An ARM Program

int32_t *assemble(tokens *lines, ass_func ass_func, const char *delim)
{
  // Pass #1
	map      *symtbl  = map_new(&map_cmp_str);
	uint16_t  address = 0;
	int       labelc  = 0;
	
	for (int i = 0; i < lines->tokn; i++)
	{
		tokens *line  = tokenize(lines->toks[i], delim);
		char   *label = line->toks[0];
		
		if (strstr(label, ":")) // Label encountered
		{
			labelc++;
			map_put(symtbl, strdup(label), heap_int(address));
			continue;
		}
		address += sizeof(int32_t);
		toks_free(line);
	}
	
	
  // Initialize Assembly Program
	int line_tot = lines->tokn - labelc;
	ass_prog *p  = malloc(sizeof(ass_prog));
	p->line_tot  = line_tot;
	p->instrs    = realloc_instrs(p);
	p->sym_tbl   = symtbl;
	p->curr_addr = 0;

	// Pass #2
	for (int i = 0; i < lines->tokn; i++)
	{
		tokens *line = tokenize(lines->toks[i], delim);
		char   *mnem = line->toks[0];
		
		if (strstr(mnem, ":")) continue; // Label encountered

		int32_t word = ass_func(line, p);
		assert(ass_prog_insert(p, word, i) == p->curr_addr);
		p->curr_addr += sizeof(int32_t);
		
		toks_free(line);
	}
	
	// Clean Up And Return
	int32_t *binary_code = ass_prog_gen(p);
	ass_prog_free(p);
	return binary_code;
}

////////////////////////////////////////////////////////////////////////////////
