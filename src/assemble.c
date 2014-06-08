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

////////////////////////////////////////////////////////////////////  PROTOTYPES

tokens *read_assembly_file(const char *);

int32_t assemble_function(tokens *, ass_prog *);

void write_object_code(int32_t *, const char *);

int32_t assemble_single_data_transfer(tokens *, ass_prog *);

//////////////////////////////////////////////////////////////////////////  MAIN

int main(int argc, char **argv)
{
	/*if (argc < 3)
	{
		fprintf(stderr, "%s\n", "Please provide input and output files!");
		exit(EXIT_FAILURE);
	}
	
	tokens *lines = read_assembly_file(argv[1]); if (DEBUG_) toks_print(lines);
	
	int32_t *code = assemble(lines, &assemble_function, " ,");
	
	write_object_code(code, argv[2]);*/

	tokens *lines = read_assembly_file("/Users/Zeme/ARM11/arm11_1314_testsuite/test_cases/add01.s");
	//if (DEBUG_) toks_print(lines);
	
	int32_t *code = assemble(lines, &assemble_function, " ,");
	
	//write_object_code(code, argv[2]);

	
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////  READ ASSEMBLY FILE

tokens *read_assembly_file(const char *path)
{
	FILE *file = NULL;
	
	file = fopen(path, "rt");
	fseek(file, 0, SEEK_END);

	long bytes = ftell(file);
	rewind(file);
	
	char buffer[bytes];
	fread(buffer, 1, bytes, file);
	buffer[bytes-1] = '\0';					//if (DEBUG_) printf("%s\n", buffer);
	
	return tokenize(buffer, "\n");
}

////////////////////////////////////////////////////////////////////////////////
////    ALL INSTRUCTIONS    ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef int idx;

/* Returns whether or not the given token is of the form <#expression> */
#define IS_EXPRESSION(tok) (tok[0] == '#' || tok[0] == '=')

/*
 * Computes the value of a register given the position/index of its token in
 * line->toks array. If R == -1 it means that the register requested is
 * ignored by the instruction and can therefore by any value (we pick 0).
 * If R is equal to the value of PC then its value is returned.
 */
#define PARSE_REG(R)\
	(((R) == -1) ? 0 : (((R) == PC) ? PC : atoi(line->toks[R]+1))) // +1 removes 'r'

/* Converts a string to a number. Works equally well for Hex and Dec numbers */
#define PARSE_EXPR(tok) ((int) strtol(tok+1, NULL, 0)) // +1 removes # or =

////////////////////////////////////////////////////////////////////////////////
////    DATA PROCESSING    /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
 * Interprest Operand2 as a shifted register and computes its value
 */
int as_shifted_register(tokens *line, idx Operand2) { return -1; } // Optional

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
	// TODO Compute 12-bit immediate value for tok
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
int parse_dpi_Operand2(tokens *line, idx Operand2)
{
	if (IS_EXPRESSION(line->toks[Operand2]))
	{
		return as_immediate_value(line->toks[Operand2]);
	}
	return as_shifted_register(line, Operand2);
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
int32_t assemble_data_proc(tokens *line, int S, idx Rn, idx Rd, idx Operand2)
{
	DataProcessingInstr instr;
	
	instr.Cond     = AL;
	instr._00      = 0;
	instr.Operand2 = parse_dpi_Operand2(line, Operand2);
	instr._I       = IS_EXPRESSION(line->toks[Operand2]);
	instr.OpCode   = 0; // TODO from dpi mnemonic to OpCode
	instr.S        = S;
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
	instr.A       = A;
	instr.S       = 0;
	instr.Rd      = PARSE_REG(Rd);
	instr.Rn      = PARSE_REG(Rn);
	instr.Rs      = PARSE_REG(Rs);
	instr._1001   = 9;
	instr.Rm      = PARSE_REG(Rm);
	
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
int32_t assemble_sdti_with_expression(tokens *line, ass_prog *p, char *address)
{
	static const int threshold = 0xFF;
	
	int address_value = PARSE_EXPR(address);
	if (address_value <= threshold)
	{
		// Substitute '=' with '#' (Same as address[0] = '#'?)
		line->toks[2][0] = '#';
		return assemble_mov(line, p);
	}
	
	// Place the constant in four bytes at the end of the assembly ﬁle
	int32_t last_address = ass_prog_insert(p, address_value, p->line_tot);
	
	int offset = last_address - p->curr_addr - 8; // Calcualate offset (+8?)
	
	// We now need to generate ldr Rd, [PC, #offset]
	line->toks[2] = "[PC";
	line->toks[3] = "#offset]"; // TODO
	
	// Call the assembly instruction
	return assemble_single_data_transfer(line, p);
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
 */
int32_t assemble_single_data_transfer(tokens *line, ass_prog *p)
{

	char *address = line->toks[2];
	
	if (IS_EXPRESSION(address))
	{
		return assemble_sdti_with_expression(line, p, address);
	}
		
	// Because of the way a pre-indexed address is specified, if the last
	// character of the last token is ']' then we have pre-idexed addressing.
	char *last_tok = line->toks[line->tokn-1];
	int PreIndexed = last_tok[strlen(last_tok)] == ']';
	
	// Now we want to get rid of annoying [ ] brakets so lets retokenize the line
	int Rn		 = 0; // TODO remove brakets and parse
	int Offset = 0; // TODO remove last braken and as_immediate_value() PARSE_EXPR(line->toks[3]);
	
	// We have a case of lrd/str Rd, [Rn], using base register Rn, with an offset of zero.
	if (line->tokn == 3)
	{
		Offset = 0;
	}
	
	SingleDataTransferInstr instr;
	
	instr.Cond   = AL;
	instr._01    = 1; // 2?
	instr._I     = 1; // We don't support shift registers syntax yet
	instr.P      = PreIndexed;
	instr.U      = 0; // Not supported yet
	instr._00    = 0;
	instr.L      = strcmp(line->toks[0], "ldr") == 0; // Set if load
	instr.Rn     = Rn;
	instr.Rd     = PARSE_REG(1);
	instr.Offset = Offset;
	
	return *((int32_t *) &instr);
}

////////////////////////////////////////////////////////////////////////////////
////    ADNEQ INSTRUCTIONS    //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int32_t andeq_instr(char **toks, ass_prog *p)  // andeq
{
	return 0x00000000;
}

////////////////////////////////////////////////////////////////////////////////
////    ASSEMBLE FUNCTION    ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
////    WRITE OBJECT CODE    ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void write_object_code(int32_t *code, const char *path)
{
	// TODO IMPLEMENT
	
	// We no longer need code
	free(code);
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
