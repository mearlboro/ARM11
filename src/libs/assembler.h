#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include "tokens.h"
#include "map.h"


typedef struct bin_instr  // Binary Instruction
{
	int32_t  bin_word;      // Binary Word
	uint16_t word_addr;     // Word Address
} bin_instr;


typedef struct ass_prog   // Assembly Program
{
	bin_instr **instrs;     // Binary Instruction Array
	map        *sym_tbl;    // Symbol Table
	int         line_tot;   // Line Total
	uint16_t    curr_addr;  // Address Of Current Assembly Instruction
} ass_prog;


typedef int32_t (*ass_func)(tokens *, ass_prog *);

int32_t *ass_prog_gen(ass_prog *);

uint16_t ass_prog_append(ass_prog *, int32_t);

void ass_prog_free(ass_prog *);

ass_prog *assemble(tokens *, ass_func, const char *);


void ass_prog_print(ass_prog *);


#endif


 
// /*
// * An assembly instruction comprises an operation mnemonic (e.g. add, ldr, ...),
// * and one, two, three or four operand ﬁelds, depending on the instruction type.
// *
// * The operation mnemonic and operands are stored in <toks>
// *
// * The position of this assembly instruction in the assembly file from which
// * is was read is stored in <lineno>, with 0 being the first line in the file.
// *
// * Each assembly instruction will be assembled into a binary instruction,
// * stored in <bin_instr>.
// */
//typedef struct ass_instr
//{
//	tokens    *toks;
//	int				 lineno;
//	bin_instr *bin_instr;
//} assembly_instr;							// DON'T NEED?
//
///*
// * An assembly directive, optionally preceded by a label.
// */
//typedef struct ass_dir { } ass_dir;
//

///*
// * An assembly program consists of a long list of binary words and relative
// * machine addresses. These are stored in <bin_instrs>
// *
// * We would like to know how many lines (words) our assembly program currently
// * has; this is a job for <line_tot>.
// *
// * Each (non-empty) line of an assembler ﬁle contains either an assembly
// * instruction or an assembler directive, optionally preceded by a label.
// * These lines are stored as tokens in <lines>.
// *
// * Labels are strings that begin with an alphabetical character (a–z or A–Z)
// * and end with a :, as in “label:”. The value of the label is the address of
// * the machine word corresponding to the position of the label. So we need a
// * way to map labels to addresses. Enter <symtbl>, a map (duh).
// *
// * Supposed we have loaded an assembly source file and we want to generate
// * an assembly program. We need a way to keep track of the program state
// * as we process and assemble it. Since we are generating the program line by
// * line, it will be nice to know the current line in the assembly file that we
// * are processing. This is stored int <curr_line>.
// *
// */
//typedef struct ass_prog
//{
//	bin_instr **bin_instrs;
//	map				 *symtbl;
//	int					line_tot;
//} ass_prog;


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

