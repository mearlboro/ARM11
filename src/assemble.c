#define _GNU_SOURCE // Needed for the compiler not to complain about asprintf

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "libs/utils.h"
#include "libs/tokens.h"
#include "libs/map.h"
#include "libs/bitwise.h"

////  INSTRUCTION AND MNEMOTICS DATATYPES  /////////////////////////////////////

//#include "libs/mnemonic.h"
#include "libs/instruction.h"
#include "libs/assembler.h"
#include "libs/ARM.h"


////////////////////////////////////////////////////////////////////////////////
////  1. MACROS AND HELPERS   //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* 
 * idx symbolizes that the argument represents an index in some array rather
 * than an actual integer value.
 */
typedef int idx;


/* 
 * Returns whether or not the given token is of the form <#expression> or
 * <=expression>.
 */
#define IS_EXPRESSION(tok) (tok[0] == '#' || tok[0] == '=')

/*
 * Computes the value of a register given the position/index of its token in
 * line->toks. If R == -1 it means that the register requested is ignored by 
 * the instruction and can therefore by any value (we pick 0).
 * If R is equal to the string "PC" then the value of PC (15) is returned.
 * Otherwise we move 1 byte across the token to remove the prefix 'r' and
 * then parse the register number.
 */
#define PARSE_REG(R)\
  (((R) == -1) ? 0 \
               : (((strcmp(line->toks[R], "PC") == 0) ? PC \
                                           : atoi(line->toks[R]+1))))
/* 
 * Converts an expression string to a number. strtol works equally well for 
 * hexadecimal and decimal numbers in that it ignores the trailing '0x' in the
 * former case. Once again we move one byte acrross the token to remove the
 * trailing '#' or '='.
 */
#define PARSE_EXPR(tok) ((int) strtol((tok)+1, NULL, 0))


/*
 * read/write function prototypes
 */
tokens *read_assembly_file(const char *);

void write_object_code(assembly_prog *, const char *);


////////////////////////////////////////////////////////////////////////////////
////  1.2 ASSEMBLE FUNCTIONS    ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
 
/*
 * This array is used to store pointers to a number of functions that will
 * assemble a binary word from an assembly line.
 */
assemble_fptr function_array[9]; 


// data processing
int32_t assemble_dataproc_result(tokens *, assembly_prog*);
int32_t assemble_dataproc_mov(tokens *, assembly_prog*);
int32_t assemble_dataproc_cprs(tokens *, assembly_prog*);

// multiply
int32_t assemble_mul(tokens *, assembly_prog*);
int32_t assemble_mla(tokens *, assembly_prog*);

// branch
int32_t assemble_branch(tokens *, assembly_prog*);

// data transfer
int32_t assemble_sdti(tokens *, assembly_prog*);

// special
int32_t assemble_lsl(tokens *, assembly_prog*);
int32_t assemble_andeq(tokens *, assembly_prog*);

/*
 * Function that assigns each array member to the actual function 
 * to parse each type of instruction. There are 9 types created based on 
 * mnemonic and binary similarities.
 */
void function_array_init() 
{
	function_array[0] = assemble_dataproc_result;
	function_array[1] = assemble_dataproc_cprs;
	function_array[2] = assemble_dataproc_mov;

	function_array[3] = assemble_mul;
	function_array[4] = assemble_mla;

	function_array[5] = assemble_branch;

	function_array[6] = assemble_sdti;

	function_array[7] = assemble_lsl;
	function_array[8] = assemble_andeq;
}


////////////////////////////////////////////////////////////////////////////////
////   2. CORE    //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * This is the main assembler function passed to assemble() from assembler.h.
 * It is invoked for each line of assembly code read from the file and for each
 * line it should assemble a binary word represeting that instruction.
 * <line> is guaranted to be an assembly instruction and not a directive (label)
 */
int32_t assembler_function(tokens *line, assembly_prog *p)
{
	char *mnemonic = line->toks[0];
	int i = str_to_mnemonic(mnemonic);
	return function_array[i](line, p);
}


/* Reads an assembly file and breaks it into token where each token is one line
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


/*
 * Given binary object code for an ARM machine, write the program onto the file
 * specified by the file path provided.
 */
void write_object_code(assembly_prog *p, const char *path)
{
	FILE *file;
	file = fopen(path, "wb");
	
	int32_t *code = assembly_prog_gen(p);
	
	size_t bytes = p->line_tot * sizeof(int32_t);
	
	assert(fwrite(code, 1, bytes, file) == bytes);

	fclose(file);
	
	// We no longer need code
	free(code);
}


////////////////////////////////////////////////////////////////////////////////
////   2.1 MAIN    /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "%s\n", "Please provide input and output files!");
		exit(EXIT_FAILURE);
	}
	
	function_array_init();

	// Obtain lines of assembly code as tokens
	tokens *lines = read_assembly_file(argv[1]);
	
	// Assemble the lines using the assembler_function
	assembly_prog *p = assemble(lines, &assembler_function, " ,");
	
	// Write the words to the output file
	write_object_code(p, "out");
	
	toks_free(lines);
	assembly_prog_free(p);

	return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
////  INSTRUCTIONS  ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////  DATA PROCESSING  /////////////////////////////////////////////////////////

/*
 * Interprets a sequence of tokens as a shifted register and computes its value.
 *
 * We are trying to parse a sequence of tokens of the form Rm{,<shift>}, where:
 *  - Rm is the index register
 *  - The {,<shift>} part is optional, but if present <shift> can be one of:
 *    1) <shiftname> <register>
 *    2) <shiftname> <#expression>
 *	  Where shiftname can be one of: asr, lsl, lsr, ror
 *
 * Rm - Is the position/index/idx of the Rm token in the line->toks array
 *
 * If Rm is equal to the number of tokens in line, then the {,<shift>} part is
 * not present and we can simply return the parsed value of the register.
 */
int as_shifted_register(tokens *line, idx Rm)
{
	if (line->tokn == Rm + 1) // Case: Rm ({,<shift>} part is not present)
	{
		return PARSE_REG(Rm);
	}
	
	char *shift_name = line->toks[Rm + 1];
	char *shift_tok  = line->toks[Rm + 2];
	
	int Rn         = PARSE_REG(Rm - 1);
	int shift      = 0;
	int shift_type = str_to_shift(shift_name);
 
	if (IS_EXPRESSION(shift_tok)) // Case: <shift> = <shiftname> <#expression>
	{
	  int shift_value = PARSE_EXPR(shift_tok);
	  shift = shift_type << 1;
	  shift = (shift_value << 3) | shift;  
	}
	else // case <shift> = <shiftname> <register>
	{
	  int shift_reg = PARSE_REG(4);
	  shift = (shift_type << 1) | 1;
	  shift = (shift_reg << 4) | shift;
	}
	
	return (shift << 4) | Rn;
}

/*
 * Converts a numeric constant into a 12-bit [Rotate+Imm] format.
 *
 * This 12-bit patters consists of an unsigned 8-bit immediate value (Imm)
 * preceeded by a 4-bit rotate field (Rotate).
 *
 * When the emulator interprests this value, the (Imm) field will be 
 * zero-extended to 32 bits, and then rotated to the right twice the number of
 * positions as specified in the Rotate field.
 *
 * It may be possible that the constant supplied cannot be represented in the
 * format specified above. If that is the case the program halts execution with
 * an error.
 */
int as_immediate_value(int constant)
{
	int no_rot = 0;
	
	// While the bits from 8 to 32 are not all 0, rotate the constant to the left
	// The number of rotations is always even as defined in the spec.
	while (bits_get(constant, 8, 31) != 0 && no_rot < 32)
	{
	  constant = rotate_left(constant, 2);
	  no_rot +=2;
	}
	if (no_rot == 32)
	{
	  perror("Cannot convert numeric constant into 12-bit");
	  exit(EXIT_FAILURE);
	}
	
	// Now we have:
	//  - Rotate = no_rot / 2 (bits 8 to 11 in Operand2)
	//  - Imm    = constant (as an 8 bit value) (bits 0 to 7 in Operand2)
	return ((no_rot / 2) << 8) | constant; // Constructing Operand2 (Rotate + Imm)
}

/* 
 * This function determines whether Operand2 is an expression or a shifted
 * register and computes and returns its value.
 */
int parse_dpi_Operand2(tokens *line, idx Operand2_i)
{
	char *Operand2 = line->toks[Operand2_i];
	
	if (IS_EXPRESSION(Operand2))
	{
		return as_immediate_value(PARSE_EXPR(Operand2));
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
	char *Operand2 = line->toks[Operand2_i];
	char *mnemonic = line->toks[0];
	
	DataProcessingInstr instr;
	
	instr.Cond     = AL;
	instr._00      = 0;
	instr.Operand2 = parse_dpi_Operand2(line, Operand2_i);
	instr._I       = IS_EXPRESSION(Operand2);
	instr.OpCode   = str_to_opcode(mnemonic); printf("%d %s\n",instr.OpCode, mnemonic);
	instr.S	       = S;
	instr.Rn       = PARSE_REG(Rn);
	instr.Rd       = PARSE_REG(Rd);
	
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
int32_t assemble_dataproc_result(tokens *line, assembly_prog *p)
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
int32_t assemble_dataproc_mov(tokens *line, assembly_prog *p)
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
int32_t assemble_dataproc_cprs(tokens *line, assembly_prog *p)
{
	return assemble_data_proc(line, 1, 1, -1, 2);
}

////    MULTIPLY    ////////////////////////////////////////////////////////////

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
int32_t assemble_mul(tokens *line, assembly_prog *p) // mul
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
int32_t assemble_mla(tokens *line, assembly_prog *p)
{
	return assemble_multiply(line, 1, 1, 2, 3, 4);
}


////    SINGLE DATA TRANSFER    ////////////////////////////////////////////////

/* Assemble single data transfer instruction prototype */
int32_t assemble_sdti(tokens *, assembly_prog *);

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
int32_t assemble_sdti_expr(tokens *line, assembly_prog *p)
{
	static const int threshold = 0xFF;
	
	char *Rd_tok   = line->toks[1];
	char *addr_tok = line->toks[2];
	int address    = PARSE_EXPR(addr_tok);
	
	if (address <= threshold) // Case: assemble as mov instruction
	{
		addr_tok[0]   = '#'; // Substitute '=' with '#' and 'ldr' with 'mov'
		line->toks[0] = strdup("mov"); // Need strdup for when freeing
		return assemble_dataproc_mov(line, p);
	}
	
	// Place the constant in four bytes at the end of the assembly file
	uint16_t last_address = assembly_prog_append(p, address);
	
	int Offset = last_address - p->curr_addr - 8 ; // Calcualate offset

	// We now need to generate ldr Rd, [PC, #offset] from ldr Rd,=offset
	char *newline = NULL;
	asprintf(&newline, "ldr %s, [PC, #%d]", Rd_tok, Offset);
	tokens *newtoks = tokenize(newline, " ,"); // TODO free up

	//free(line);
	
	return assemble_sdti(newtoks, p);
}

/*
 * Assembles a single data transfer instruction.
 *
 * Syntax: <ldr/str> Rd, <address>.
 *   <ldr/str> - Is the instruction mnemonic, either load or store instruction
 *   Rd        - Represent the destination register (r0, r1, PC, ...)
 *   <address> - Can take one of several forms:
 *     1) A numeric constant of the form: <=expression> 
 *     2) Pre-Indexed address specification:
 *				  (i) [Rn] - Eventually interpreted as [Rn, #0]
 *         (ii) [Rn, <#expression>] - Base register Rn, offset #expression bytes
 *        (iii) [Rn,{+/-}Rm{,<shift>}] - Using shifted registers
 *     3) Post-Indexed address specification:
 *          (i) [Rn], <#expression> - Base register Rn, offset #expression bytes
 *         (ii) [Rn,{+/-}Rm{,<shift>}] - Using shifted registers
 *
 * Because of the way a pre-indexed address is specified, if the last
 * character of the last token is ']' then we have pre-idexed addressing.
 *
 * We will want to get rid of annoying [] brakets so we will retokenize the line
 *
 * Immediate flag (I) is CLEAR when Operand2 is a numeric constant.
 * L flag will be set if the instruction is a load (lrd) instruction.
 */
int32_t assemble_sdti(tokens *_line, assembly_prog *p)
{
	char *address = _line->toks[2];
	if (IS_EXPRESSION(address)) // Case when address is an expression
	{
		return assemble_sdti_expr(_line, p);
	}
	
	char *mnemonic = _line->toks[0];
	int PreIndexed = toks_endc(_line) == ']';
	
	tokens *line     = tokenize(strdup(_line->line), " ,[]");
	char *offset_tok = line->toks[3];
	
	int Offset        = 0;
	int ImmediateFlag = 0;
	int UpFlag        = 1;
	
	if (line->tokn == 3) // Case: Rn --> Rd, Rn, #0
	{
		Offset = 0;
	}
	else if (IS_EXPRESSION(offset_tok)) // Case: Rn, <#expression>
	{
		Offset = PARSE_EXPR(offset_tok);
		UpFlag = Offset >= 0;
	}
	else // Case: Rn,{+/-}Rm{,<shift>}
	{
		ImmediateFlag = 1;
		// See if {+/-} sign is present
		int has_sign = offset_tok[0] == '-' || offset_tok[0] == '+';
		// Remove {+/-} sign if present
		if (has_sign)
		{
			UpFlag = (offset_tok[0] == '+');
			offset_tok++;
		}
		// Now compute shifted register
		Offset = as_shifted_register(line, 3);
		// Work out UpFlag accordingly
		UpFlag = (has_sign) ? UpFlag : (Offset >= 0);
	}
	
	SingleDataTransferInstr instr;
	
	instr.Cond   = AL;
	instr._01    = 1;
	instr._I     = ImmediateFlag;
	instr.P	     = PreIndexed;
	instr.U	     = UpFlag;
	instr._00    = 0;
	instr.L	     = strcmp(mnemonic, "ldr") == 0;
	instr.Rn     = PARSE_REG(2);
	instr.Rd     = PARSE_REG(1);
	instr.Offset = ABS(Offset);
	
	toks_free(line); // Clean up
	
	return *((int32_t *) &instr);
}

////    BRANCH INSTRUCTIONS    /////////////////////////////////////////////////

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
int32_t assemble_branch(tokens *line, assembly_prog *p)
{	
	char *suffix = (strcmp(line->toks[0], "b") == 0) ? "al" 
                                                   : (line->toks[0] + 1);
	char *label  = line->toks[1];
	
	uint16_t lbl_addr = *(uint16_t *) map_get(p->sym_tbl, label);
	
	int sign   = (lbl_addr < p->curr_addr) ? -1 : 1;
	int Offset = (sign * (p->curr_addr - lbl_addr + 8)) >> 2;
	
	BranchInstr instr;
	
	instr.Cond   = str_to_cond(suffix);
	instr._1010  = 10;
	instr.Offset = Offset;

	return *(int32_t *) &instr;
}

////    ADNEQ INSTRUCTION    ///////////////////////////////////////////////////

int32_t assemble_andeq(tokens *line, assembly_prog *p)
{
	return 0x00000000;
}

////////////////////////////////////////////////////////////////////////////////
////    LSL INSTRUCTION    /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int32_t assemble_lsl(tokens *line, assembly_prog *p)
{
	//transform lsl Rn, <#expression> to mov Rn,Rn, lsl <#expression>.
	char *newline = NULL;
	asprintf(&newline, "mov %s,%s, lsl %s", line->toks[1],
	                                        line->toks[1],
	                                        line->toks[2]);
	tokens *newtoks = tokenize(newline, " ,"); // TODO free up

	return assemble_dataproc_mov(newtoks, p);
}
	
	



