#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "tokens.h"
#include "map.h"
#include "utils.h"
#include "assembler.h"
#include "bitwise.h"


//////////////////////////////////////////////////////  Free An Assembly Program

void assembly_prog_free(assembly_prog *p)
{
	for (int i = 0; i < p->line_tot; i++)
	{
		free(p->instrs[i]);
	}
	free(p->instrs);
	map_free(p->sym_tbl, MAP_FREE_VAL | MAP_FREE_KEY);
	free(p);
}

///////////////////////////////////////////////  Extend Binary Instruction Array

static void realloc_instrs(assembly_prog *p)
{
	p->instrs = mem_chk(realloc(p->instrs, sizeof(bin_instr *) * (p->line_tot)));
}

//////////////////////////////////////////////////////  Generate ARM-Binary Code

int32_t *assembly_prog_gen(assembly_prog *p)
{
	int32_t *words = mem_chk(malloc(sizeof(int32_t) * p->line_tot));
	for (int i = 0; i < p->line_tot; i++)
	{
		words[i] = p->instrs[i]->bin_word;
	}
	return words;
}

////////////////////////////////////////////////////////////  Insert Binary Word

uint16_t assembly_prog_append(assembly_prog *p, int32_t word)
{
	// Appending to end of program
	p->line_tot++;
	realloc_instrs(p);

	uint16_t address = (p->line_tot-1) * sizeof(int32_t); 
	bin_instr *instr = mem_chk(malloc(sizeof(bin_instr)));
	instr->bin_word  = word;
	instr->word_addr = address;

	int curr_instr = address / sizeof(int32_t);
	p->instrs[curr_instr] = instr;

	return address;
}

static void assembly_prog_write(assembly_prog *p, int32_t word)
{
	printf("%08x : ", p->curr_addr); print_bits_BE(word);

	bin_instr *instr = mem_chk(malloc(sizeof(bin_instr)));
	instr->bin_word  = word;
	instr->word_addr = p->curr_addr;

	int curr_instr = p->curr_addr / sizeof(int32_t);
	p->instrs[curr_instr] = instr;
	p->curr_addr += sizeof(int32_t);

}

///////////////////////////////////////////////////////  Assemble An ARM Program

assembly_prog *assemble(tokens *lines, assemble_fptr f, const char *delim)
{
  // Pass #1
	map      *symtbl  = map_new(&map_cmp_str);
	uint16_t  address = 0;
	int       labelc  = 0;

	for (int i = 0; i < lines->tokn; i++)
	{
		char   *currl = strdup(lines->toks[i]);
		tokens *line  = tokenize(currl, delim);
		char   *label = line->toks[0];

		if (strchr(label, ':')) // Label encountered
		{
			labelc++;
			map_put(symtbl, strdelchr(label, ':'), heap_uint16_t(address));
            toks_free(line);
			continue;
		}
		address += sizeof(int32_t);

		toks_free(line);
	}

  // Initialize Assembly Program
	int line_tot      = lines->tokn - labelc;
	assembly_prog *p  = mem_chk(malloc(sizeof(assembly_prog)));
	p->instrs         = mem_chk(malloc(0));
	p->line_tot       = line_tot;
	p->sym_tbl        = symtbl;
	p->curr_addr      = 0;
	realloc_instrs(p);

	// Pass #2
	for (int i = 0; i < lines->tokn; i++)
	{
		char  *currl = strdup(lines->toks[i]);
		tokens *line = tokenize(currl, delim);
		char   *mnem = line->toks[0];

		if (strchr(mnem, ':')) continue; // Label encountered

		int32_t word = f(line, p);
		assembly_prog_write(p, word);

		toks_free(line);
	}
	// Assembling done
	return p;
}

//////////////////////////////////////////////////// Prints an assembler program

void assemble_prog_print(assembly_prog *p)
{
	for (int i = 0; i < p->line_tot; i++)
	{
		uint8_t add = p->instrs[i]->word_addr;
		int32_t word = p->instrs[i]->bin_word;
		printf("%07x: ", add); print_bits_BE(word);
	}
}


