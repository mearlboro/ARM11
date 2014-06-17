#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include <stdint.h>
#include "tokens.h"
#include "map.h"


typedef struct bin_instr  // Binary Instruction
{
	int32_t  bin_word;      // Binary Word
	uint16_t word_addr;     // Word Address
} bin_instr;


typedef struct assembly_prog   // Assembly Program
{
	bin_instr **instrs;     // Binary Instruction Array
	map        *sym_tbl;    // Symbol Table
	int         line_tot;   // Line Total
	uint16_t    curr_addr;  // Address Of Current Assembly Instruction
} assembly_prog;


typedef int32_t (*assemble_fptr)(tokens *, assembly_prog*);

//// CORE ASSEMBLER FUNCTIONS ///////////

int32_t *assembly_prog_gen(assembly_prog *);

uint16_t assembly_prog_append(assembly_prog *, int32_t);

void assembly_prog_free(assembly_prog *);

assembly_prog *assemble(tokens *, assemble_fptr, const char *);

void assembly_prog_print(assembly_prog *);

#endif


 
/*
 * An assembly program consists of a long list of binary words and relative
 * machine addresses. These are stored in <bin_instrs>
 *
 * We would like to know how many lines (words) our assembly program currently
 * has. This is a job for <line_tot>.
 *
 * Labels are strings that begin with an alphabetical character (a–z or A–Z)
 * and end with a :, as in “label:”. The value of the label is the address of
 * the machine word corresponding to the position of the label. So we need a
 * way to map labels to addresses. Enter <symtbl>, a map (duh).
 *
 * There is an important aspect to keep in mind. Some assembly instructions
 * (i.e. ldr, str, ...) will want to append a new binary word to the end
 * of the assembly program. An assembly program thus consists of a series of
 * words each translating to an assembly instruction, followed by an arbitrary
 * number of words representing constant values to load/store from/to memory.
 * When assembling he program, we want to have the possibility of appending
 * such constants to the end of the assembly program.
 *
 * At the current state this is a two-pass assembly. In the first pass, each
 * label is mapped to its corresponding address. This way the total value
 * of 'pure' assembly instructions is found as number of assembly lines
 * in the file minus number of labels.
 *
 * When creating a new ass_prog using ass_prog_new, ass_fun will be invoked
 * for each non-label assembly instruction passing an ass_prog variable
 * representing the current state of the assembly program.
 * This will have <line_tot> set to the number of 'pure' assembly instructions
 * determined in the way explained above. This is getting really verbose, it's
 * 5:19am I should get some sleep. Anyways. We mentioned how ldr/str will
 * want to append 'constant-value-word-instructions' to the end of the
 * assemply program. This can be done by passing <ass_prog->line_tot> as
 * the <pos> parameter in <ass_prog_insert>, causing <line_tot> to be
 * incremented. So how do we simply write instructions? We use <i> in the
 * for loop. This really went downhill: what I have just writte, this
 * wall of text, is practically useless to anybody who is not me.
 */

