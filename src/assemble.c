#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "libs/utils.h"
#include "libs/tokens.h"
#include "libs/map.h"
#include "libs/bitwise.h"
#include "libs/assembler.h"
#include "instructions.h"

////////////////////////////////////////////////////////////////////////////////
////    ALL INSTRUCTIONS    ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* 
 * Not really needed, just symbolizes that the argument represents an index
 * in some array and not an actual integer value.
 */
typedef int idx;

/* Returns whether or not the given token is of the form <#expression> */
#define IS_EXPRESSION(tok) (tok[0] == '#' || tok[0] == '=')

/*
 * Computes the value of a register given the position/index of its token in
 * line->toks array. If R == -1 it means that the register requested is
 * ignored by the instruction and can therefore by any value (we pick 0).
 * If R is equal to the value of PC then the value of PC (15) is returned.
 */
#define PARSE_REG(R)\
  (((R) == -1) ? 0 \
               : (((*line->toks[R] == 'P') ? PC \
                                           : atoi(line->toks[R]+1))))

/* Converts a string to a number. Works equally well for Hex and Dec numbers */
#define PARSE_EXPR(tok) ((int) strtol((tok)+1, NULL, 0)) // +1 removes # or =

////////////////////////////////////////////////////////////////////////////////
////    DATA PROCESSING    /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * Interprest Operand2 as a shifted register and computes its value
 *
 * Operand2 will be of the form Rm{,<shift>}, where the <shift> part is optional
 * <Operand2> argument is the index of Operand2 in tokens. Therefore if
 * <line->tokn> < (Operand2+1) then we have a shifted register, otherwise
 * parsed operand2 will only have Rm and then zeros.
 */
int as_shifted_register(tokens *line, idx Operand2)
{
	if (line->tokn <= Operand2+1)
	{
		return PARSE_REG(Operand2);
	}
	return -1; // Optional, not supported for now
}

/*
 * Converts a numeric constant into a 12-bit Operand2/Offset value, as defined
 * in the data processing instruction format (Rotate + Imm).
 *
 * The tokene represeting the numeric constant can either be in decimal or
 * hexadecimal format (0x prefix for hexadecimal).
 */
int as_immediate_value(char *tok)
{
	int constant = PARSE_EXPR(tok);
	int no_rot = 0;
	// while the bits from 8 to 32 are not all 0, rotate the constant
	// no of rotations is always even as in the specs
	while (bits_get(constant,8,31) != 0 && no_rot < 32)
	{
	  constant = rotate_right(constant,2);
	  no_rot +=2;
	}
	if (no_rot == 32)
	{
	  perror("Cannot convert numeric constant into 12-bit");
	  exit(EXIT_FAILURE);
	}
	
	// Now we have
	// Rotate = no_rot / 2 ( bits 8 to 11 in Operand2)
	// Imm    = constant ( as an 8 bit value) (bits 0 to 7 in Operand2)
	no_rot /= 2;
	constant = (no_rot << 8) | constant; // constructing Operand2 (Rotate + Imm)
	
	return constant;
}

/* 
 * Operand2 in a data processing instruction can take two forms:
 *   <#expression> - Represents a numeric constant to be converted to a 12-bit
 *                   immediate value according to the DPI format (Rotate + Imm)
 *   Rm{,<shift>}  - Represents a shifted register (optional)
 *
 * This function determines whether Operand2 is an expression or a shifted
 * register and computes and returns its value.
 */
int parse_dpi_Operand2(tokens *line, idx Operand2_i)
{
	char *Operand2 = line->toks[Operand2_i];
	
	if (IS_EXPRESSION(Operand2))
	{
		return as_immediate_value(Operand2);
	}
	return as_shifted_register(line, Operand2_i);
}

/*
 * Assembles a data processing instruction.
 * 
 * The syntax for the three main 'types' of data processing instructions is:
 *   <opcode>   - Is the instruction mnemonic associated with an integer OpCode
 *   Rd, Rn, Rm - Represent registers (r0, r1, PC, ...)
 *   <Operand2> - Represents an operand. It can be interpreted as a numeric 
 *                constant or a shifted register
 *
 * line     - Contains the tokens forming this instruction
 * S        - Represents whether or not the CPRS flags are to be set
 * Rn       - Is the position/index/idx of the Rn token in the line->toks array
 * Rs       - Similarly, for Rs
 * Operand2 - Similarly, for Operand2
 *
 * Immediate flag (I) is set when Operand2 is a numeric constant.
 */
int32_t assemble_data_proc(tokens *line, int S, idx Rn, idx Rd, idx Operand2_i)
{
	//////////////////////////  Data processing instruction mnemonic to OpCode map
	static map *dpimne_opcode = NULL;
	if (dpimne_opcode == NULL)
	{
		map *m = map_new(&map_cmp_str);
		map_put(m, "and", heap_int(AND));
		map_put(m, "eor", heap_int(EOR));
		map_put(m, "sub", heap_int(SUB));
		map_put(m, "rsb", heap_int(RSB));
		map_put(m, "add", heap_int(ADD));
		map_put(m, "tst", heap_int(TST));
		map_put(m, "teq", heap_int(TEQ));
		map_put(m, "cmp", heap_int(CMP));
		map_put(m, "orr", heap_int(ORR));
		map_put(m, "mov", heap_int(MOV));
		dpimne_opcode = m;
	}
	//////////////////////////////////////////////////////  EVENTUALLY CHANGE THIS
	
	char *Operand2 = line->toks[Operand2_i];
	char *mnemonic = line->toks[0];
	
	DataProcessingInstr instr;
	
	instr.Cond     = AL;
	instr._00			 = 0;
	instr.Operand2 = parse_dpi_Operand2(line, Operand2_i);
	instr._I 			 = IS_EXPRESSION(Operand2);
	instr.OpCode   = *(int *) map_get(dpimne_opcode, mnemonic);
	instr.S				 = S;
	instr.Rn			 = PARSE_REG(Rn);
	instr.Rd			 = PARSE_REG(Rd);
	
	return *((int32_t *) &instr);
}

/*
 * Assembles a data processing instruction that computes results.
 *
 * (and, eor, sub, rsb, add, orr)
 * Syntax: <opcode> Rd, Rn, <Operand2>
 * 
 * CPRS flags are not set, hence S is clear.
 * Positions of tokens Rn, Rd and Operand2 are 2, 1, and 3 respectively.
 */
int32_t assemble_dpi_result(tokens *line, ass_prog *p)
{
	return assemble_data_proc(line, 0, 2, 1, 3);
}

/*
 * Assembles a single operand assignment (mov) instruction.
 * Syntax: mov Rd, <Operand2> { Rn ignored }
 *
 * CPRS flags are not set, hence S is clear.
 * Positions of tokens Rd and Operand2 are 1 and 2 respectively.
 * Rn is ignored therefore -1 is passed as the 'position' of token Rn.
 */
int32_t assemble_mov(tokens *line, ass_prog *p)
{
	return assemble_data_proc(line, 0, -1, 1, 2);
}

/*
 * Assembles a data processing instruction that does not compute results, 
 * but does set the CPSR flags.
 *
 * (are tst, teq, cmp)
 * Syntax: <opcode> Rn, <Operand2> { Rd ignored }
 *
 * CPRS flags ARE set, hence a S is set to 1.
 * Positions of tokens Rn and Operand2 are 1 and 2 respectively.
 * Rd is ignored therefore -1 is passed as the 'position' of token Rd.
 */
int32_t assemble_dpi_cprs(tokens *line, ass_prog *p)
{
	return assemble_data_proc(line, 1, 1, -1, 2);
}

////////////////////////////////////////////////////////////////////////////////
////    MULTIPLY    ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * Assembles a multiply instruction.
 *
 * Multiply instructions are one of (mla) and (mul) depending on whether or not
 * the instruction has an accumulate register Rn.
 * Rd, Rs, Rn, Rm - Represent registers (r0, r1, PC, ...)
 *
 * line - Contains the tokens forming this instruction
 * A    - Represents whether or not this multiply instruction has an accumulate
 * Rd   - Is the position/index/idx of the Rd token in the line->toks array
 * Rm   - Similarly, for Rm
 * Rs   - Similarly, for Rs
 * Rn   - Similarly, for Rn
 */
int32_t assemble_multiply(tokens *line, int A, idx Rd, idx Rm, idx Rs, idx Rn)
{
	MultiplyInstr instr;
	
	instr.Cond    = AL;
	instr._000000	= 0;
	instr.A			  = A;
	instr.S			  = 0;
	instr.Rd			= PARSE_REG(Rd);
	instr.Rn			= PARSE_REG(Rn);
	instr.Rs			= PARSE_REG(Rs);
	instr._1001   = 9;
	instr.Rm			= PARSE_REG(Rm);
	
	return *((int32_t *) &instr);
}

/*
 * Assembles the multiply (mul) instruction.
 * Syntax: mul Rd, Rm, Rs { Rn ignored }
 *
 * This instruction doesn't accumulates the result hence A is clear.
 * Positions of tokens Rd, Rm and Rs are 1, 2 and 3 respectively.
 * Rn is ignored therefore -1 is passed as the 'position' of token Rn.
 */
int32_t assemble_mul(tokens *line, ass_prog *p) // mul
{
	return assemble_multiply(line, 0, 1, 2, 3, -1);
}

/*
 * Assembles the multiply with accumulate (mla) instruction.
 * Syntax: mla Rd, Rm, Rs, Rn
 *
 * This instruction does accumulates the result hence A is set to 1.
 * Positions of tokens Rd, Rm, Rs and Rn are 1, 2, 3 and 4 respectively.
 */
int32_t assemble_mla(tokens *line, ass_prog *p)
{
	return assemble_multiply(line, 1, 1, 2, 3, 4);
}

////////////////////////////////////////////////////////////////////////////////
////    SINGLE DATA TRANSFER    ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* Assemble single data transfer instruction prototype */
int32_t assemble_sdti(tokens *, ass_prog *);

/*
 * If <address> is of the form <=expression> then it is interpreted as a 
 * numeric constant. If its value is not above a set threshold (0xFF) then
 * this ldr instruction can be assembled as a mov instruction.
 *
 * Otherwise we put the value of <=expression> in four bytes at the end of the
 * assembled program, and use the address of this value, with the PC as the 
 * base register and a calculated offset, in the assembled ldr instruction.
 * It will be always a pre-indexed address.
 */
int32_t assemble_sdti_expr(tokens *line, ass_prog *p)
{
	static const int threshold = 0xFF;
	
	char *addr_tok = line->toks[2];
	int address = PARSE_EXPR(addr_tok);
	if (address <= threshold)
	{
		addr_tok[0] = '#'; // Substitute '=' with '#'
		return assemble_mov(line, p);
	}
	
	// Place the constant in four bytes at the end of the assembly file
	uint16_t last_address = ass_prog_append(p, address);
	
	int Offset = last_address - p->curr_addr - 8 ; // Calcualate offset (-8?)

	// We now need to generate ldr Rd, [PC, #offset] from ldr Rd,=offset
	char *newline = NULL;
	asprintf(&newline, "ldr %s, [PC, #%d]", line->toks[1], Offset);
	tokens *newtoks = tokenize(newline, " ,");

	//free(line);
	
	return assemble_sdti(newtoks, p);
}

/*
 * Assembles a single data transfer instruction.
 *
 * Syntax: <ldr/str> Rd, <address>.
 *   <ldr/str> - Is the instruction mnemonic, either load or store instruction
 *               ldr will require L bit to be set. str will have L bit clear
 *   Rd        - Represent the destination register (r0, r1, PC, ...)
 *   <address> - Can take one of several forms
 *
 * Notes:
 *   Because of the way a pre-indexed address is specified, if the last
 *   character of the last token is ']' then we have pre-idexed addressing.
 *
 * We will want to get rid of annoying [] brakets so we will retokenize the line
 *
 * Immediate flag (I) is CLEAR when Operand2 is a numeric constant.
 */
int32_t assemble_sdti(tokens *line, ass_prog *p)
{
	// Case when address is an expression
	char *address = line->toks[2];
	if (IS_EXPRESSION(address))
	{
		return assemble_sdti_expr(line, p);
	}
		
	// Every other case
	int PreIndexed = toks_endc(line) == ']';
	
	line = tokenize(line->line, " ,[]"); // Retokenize

	int Rn		 = PARSE_REG(2);
	int Offset = (line->tokn == 3) ? 0 : PARSE_EXPR(line->toks[3]);

	SingleDataTransferInstr instr;
	
	instr.Cond   = AL;
	instr._01	   = 1;
	instr._I		 = 0; // Change if/when shifted reg are supported
	instr.P			 = PreIndexed;
	instr.U			 = 1; // Change if/when shifted reg are supported
	instr._00		 = 0;
	instr.L			 = strcmp(line->toks[0], "ldr") == 0; // Set if load
	instr.Rn     = Rn;
	instr.Rd		 = PARSE_REG(1);
	instr.Offset = Offset;
	
	return *((int32_t *) &instr);
}

////////////////////////////////////////////////////////////////////////////////
////    BRANCH INSTRUCTIONS    /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * Assembles a branch instruction.
 *
 * The syntax is: b<code> <expression>
 *   <code>       - One of eq, ne, ge, lt, gt, al, or "just b"
 *   <expression> - The label name whose address is to be retrieved
 *
 * The assembler should compute the offset between the address of the current 
 * (this) instruction and that pointed by the label, taking into account the 
 * off-by-8 bytes effect that will occur due to the ARM pipeline. 
 * This signed offset should be 26 bits in length, before being shifted right 
 * two bits and having the lower 24 bits stored in the Offset field.
 */
int32_t assemble_branch(tokens *line, ass_prog *p)
{
	///////////////////////////////////////  Branch instruction suffix to code map
	static map *brnsuff_code = NULL;
	if (brnsuff_code == NULL)
	{
		map *m = map_new(&map_cmp_str);
		map_put(m, "eq", heap_int(EQ));
		map_put(m, "ne", heap_int(NE));
		map_put(m, "ge", heap_int(GE));
		map_put(m, "lt", heap_int(LT));
		map_put(m, "gt", heap_int(GT));
		map_put(m, "le", heap_int(LE));
		map_put(m, "al", heap_int(AL));
		brnsuff_code = m;
	}
	//////////////////////////////////////////////////////  EVENTUALLY CHANGE THIS
	
	char *suffix = (strcmp(line->toks[0], "b") == 0) ? "al" : (line->toks[0]+1);
	char *label  = line->toks[1];
	
	uint16_t lbl_addr = *(uint16_t *) map_get(p->sym_tbl, label);
	
	int sign   = (lbl_addr < p->curr_addr) ? -1 : 1;
	int Offset = (sign * (p->curr_addr - lbl_addr + 8)) >> 2;
	
	BranchInstr instr;
	
	instr.Cond   = *(int *) map_get(brnsuff_code, suffix);
	instr._1010  = 10;
	instr.Offset = Offset;

	return *(int32_t *) &instr;
}

////////////////////////////////////////////////////////////////////////////////
////    ADNEQ INSTRUCTION    ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int32_t assemble_andeq(tokens *line, ass_prog *p)
{
	return 0x00000000;
}

////////////////////////////////////////////////////////////////////////////////
////    LSL INSTRUCTION    /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int32_t assemble_lsl(tokens *line, ass_prog *p)
{
	// TODO 2 : Implement this

	return -1;
}

////////////////////////////////////////////////////////////////////////////////
////    MAIN ASSEMBLY FUNCTION    //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * This is the main assembler function passed to assemble() from assembler.h.
 * It is invoked for each line of assembly code read from the file and for each
 * line it should assemble a binary word represeting that instruction.
 * <line> is guaranted to be an assembly instruction and not a directive (label)
 */
int32_t assembler_function(tokens *line, ass_prog *p)
{
	///////////////////  Function mnemonic to corresponding assembler function map
	static map *ass_funcs = NULL;
	if (ass_funcs == NULL)
	{
		ass_funcs = map_new(&map_cmp_str);
		map_put(ass_funcs, "add", &assemble_dpi_result);
		map_put(ass_funcs, "sub", &assemble_dpi_result);
		map_put(ass_funcs, "rsb", &assemble_dpi_result);
		map_put(ass_funcs, "and", &assemble_dpi_result);
		map_put(ass_funcs, "eor", &assemble_dpi_result);
		map_put(ass_funcs, "orr", &assemble_dpi_result);
		map_put(ass_funcs, "mov", &assemble_mov);
		map_put(ass_funcs, "tst", &assemble_dpi_cprs);
		map_put(ass_funcs, "teq", &assemble_dpi_cprs);
		map_put(ass_funcs, "cmp", &assemble_dpi_cprs);
		map_put(ass_funcs, "mul", &assemble_mul);
		map_put(ass_funcs, "mla", &assemble_mla);
		map_put(ass_funcs, "ldr", &assemble_sdti);
		map_put(ass_funcs, "str", &assemble_sdti);
		map_put(ass_funcs, "beq", &assemble_branch);
		map_put(ass_funcs, "bne", &assemble_branch);
		map_put(ass_funcs, "bge", &assemble_branch);
		map_put(ass_funcs, "bgt", &assemble_branch);
		map_put(ass_funcs, "ble", &assemble_branch);
		map_put(ass_funcs, "b"  , &assemble_branch);
		map_put(ass_funcs, "lsl", &assemble_lsl);
		map_put(ass_funcs, "andeq", &assemble_andeq);
	}
	//////////////////////////////////////////////////////  EVENTUALLY CHANGE THIS
	
	char *mnemonic    = line->toks[0];
	ass_func function = (ass_func) map_get(ass_funcs, mnemonic);
	
	return function(line, p);
}

////////////////////////////////////////////////////////////////////////////////
////    READ ASSEMBLY FILE    //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * Reads an assembly file and breaks it into token where each token is one line
 * of assembly code. The tokens are then returned, how nice of us!
 */
tokens *read_assembly_file(const char *path)
{
	FILE *file = NULL;
	
	file = fopen(path, "rt");
	fseek(file, 0, SEEK_END);
	
	long bytes = ftell(file);
	rewind(file);
	
	char buffer[bytes];
	fread(buffer, 1, bytes, file);
	
	// TODO: Find out why we manually need to null-terminate the program text.
	// If we don't include this line the last byte of buffer is some random char.
	buffer[bytes-1] = '\0';
	
	return tokenize(buffer, "\n");
}

////////////////////////////////////////////////////////////////////////////////
////    WRITE OBJECT CODE    ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * Given binary object code for an ARM machine, write the program onto the file
 * specified by the file path provided.
 */
void write_object_code(int32_t *code, const char *path)
{
	// (TODO NUMBER #3) : Write <code> on the file specified by <path>

	// We no longer need code
	free(code);
}

////////////////////////////////////////////////////////////////////////////////
////    MAIN    ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	/*************** FOR TESTING PURPOSES, REAL MAIN IS BELOW *******************/
	char *path = "/Users/Zeme/ARM11/arm11_1314_testsuite/test_cases/gpio_1.s";
	tokens *lines = read_assembly_file(path); toks_print(lines);
	assemble(lines, &assembler_function, " ,");
	/****************************************************************************/
	
	/*
	if (argc < 3)
	{
		fprintf(stderr, "%s\n", "Please provide input and output files!");
		exit(EXIT_FAILURE);
	}
	
	// Obtain lines of assembly code as tokens
	tokens *lines = read_assembly_file(argv[1]);
	
	// Assemble the lines using the assembler_function
	int32_t *code = assemble(lines, &assembler_function, " ,");
	
	// Write the words to the output file
	write_object_code(code, argv[2]);
	*/
	
	return EXIT_SUCCESS;
}



////////////////////////////////////////////////////////////////////////////////
/*
 {
 // Prepare men
 static map *mne_opcode = NULL;
 if (mne_opcode == NULL)
 {
 mne_opcode = map_new(&map_cmp_str);
 map_put(mne_opcode, "and", heap_int(AND));
 map_put(mne_opcode, "eor", heap_int(EOR));
 map_put(mne_opcode, "sub", heap_int(SUB));
 map_put(mne_opcode, "rsb", heap_int(RSB));
 map_put(mne_opcode, "add", heap_int(ADD));
 map_put(mne_opcode, "tst", heap_int(TST));
 map_put(mne_opcode, "teq", heap_int(TEQ));
 map_put(mne_opcode, "cmp", heap_int(CMP));
 map_put(mne_opcode, "orr", heap_int(ORR));
 map_put(mne_opcode, "mov", heap_int(MOV));
 }*/

/*
 int32_t assemble_function(tokens *line, ass_prog *p)
 {
 static map *ass_funcs = NULL;
 if (ass_funcs == NULL)
 {
 ass_funcs = map_new(&map_cmp_str);
 map_put(ass_funcs, "add", &assemble_dpi_result);
 map_put(ass_funcs, "sub", &assemble_dpi_result);
 map_put(ass_funcs, "rsb", &assemble_dpi_result);
 map_put(ass_funcs, "and", &assemble_dpi_result);
 map_put(ass_funcs, "eor", &assemble_dpi_result);
 map_put(ass_funcs, "orr", &assemble_dpi_result);
 map_put(ass_funcs, "mov", &assemble_mov);
 map_put(ass_funcs, "tst", &assemble_dpi_cprs);
 map_put(ass_funcs, "teq", &assemble_dpi_cprs);
 map_put(ass_funcs, "cmp", &assemble_dpi_cprs);
 map_put(ass_funcs, "mul", &assemble_mul);
 map_put(ass_funcs, "mla", &assemble_mla);
 map_put(ass_funcs, "ldr", NULL);
 map_put(ass_funcs, "str", NULL);
 map_put(ass_funcs, "beq", NULL);
 map_put(ass_funcs, "bne", NULL);
 map_put(ass_funcs, "bge", NULL);
 map_put(ass_funcs, "bgt", NULL);
 map_put(ass_funcs, "ble", NULL);
 map_put(ass_funcs, "b"  , NULL);
 map_put(ass_funcs, "lsl", NULL);
 map_put(ass_funcs, "andeq", NULL);
 }
 
 toks_print(line);
 
 int32_t word = ((ass_func) map_get(ass_funcs, line->toks[0]))(line, p);
 print_bits(word);
 return word;
 }
 */
 
 ////////////////////////////////////////////////////////////////////////////////
 ////    ASSEMBLE FUNCTIONS    //////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////
 
// /*
// * This array is used to store pointers to a number of functions that will
// * assemble a binary word from an assembly line.
// */
//static ass_func assemblers[] =
//{
//	assemble_dpi_result,
//	assemble_mov,
//	assemble_dpi_cprs,
//	assemble_mul,
//	assemble_mla,
//	assemble_sdti,
//	assemble_andeq,
//	assemble_lsl
//};

