#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "util/bitwise.h"
#include "util/tokens.h"
#include "util/map.h"
#include "util/utils.h"


////  INSTRUCTION AND MNEMOTICS DATATYPES  /////////////////////////////////////
#include "opcode_T.h"
#include "instruction_T.h"


////  INSTRUCTION FUNCTIONS ARRAY  /////////////////////////////////////////////

typedef int32_t (*assemble_fptr)( char**, map* );
assemble_fptr function_array[10]; 


// data processing
int32_t assemble_dataproc_result(char **, map*);
int32_t assemble_dataproc_mov(char **, map*);
int32_t assemble_dataproc_flags(char **, map*);

// multiply
int32_t assemble_mul(char **, map*);
int32_t assemble_mla(char **, map*);

// branch
int32_t assemble_branch(char **, map*);

// data transfer
int32_t assemble_ldr(char **, map*);
int32_t assemble_str(char **, map*);

// special
int32_t assemble_lsl(char **, map*);
int32_t assemble_andeq(char **, map*);

// assigns each array member to the actual function 
void function_array_init() 
{
	function_array[0] = assemble_dataproc_result;
	function_array[1] = assemble_dataproc_flags;
	function_array[2] = assemble_dataproc_mov;

	function_array[3] = assemble_mul;
	function_array[4] = assemble_mla;

	function_array[5] = assemble_branch;

	function_array[6] = assemble_ldr;
	function_array[7] = assemble_str;

	function_array[8] = assemble_lsl;
	function_array[9] = assemble_andeq;
}

////  OTHER FUNCTION PROTOTYPES  //////////////////////////////////////////////

tokens *read_assembly_file(const char *);

int32_t *assemble_program(tokens *);

void write_object_code(int32_t *, const char *);


////////////////////////////////////////////////////////////////////////////////
////  MAIN  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	tokens *lines = read_assembly_file(argv[1]); toks_print(lines);
	
	int32_t *code = assemble_program(lines);  // argv[2];
	
	write_object_code(code, argv[2]);       // argv[2];

	return EXIT_SUCCESS;
}


////  READ ASSEMBLY FILE  /////////////////////////////////////////////////////

tokens *read_assembly_file(const char *path)
{
	FILE *file = NULL;
	
	file_chk((file = fopen(path, "rt")) != NULL, "fopen");
	file_chk(fseek(file, 0, SEEK_END)   != -1,   "fseek");
	
	long bytes = ftell(file);
	rewind(file);
	
	char buffer[bytes];

	file_chk(fread(buffer, 1, bytes, file) == bytes, "fread"); 
	
	// TODO WHY DO I NEED TO DO THIS???
	buffer[bytes-1] = '\0';                                    
	tokens *lines = tokenize(buffer, "\n"); //toks_print(toks);
	return lines;
}


////  EXECUTES ASSEMBLER PROGRAM  //////////////////////////////////////////////

int32_t *assemble_program(tokens *lines)
{
	int32_t *instructions = calloc(4, lines->tokn);
	map *symbol_table = map_new(&map_cmp_str);
	function_array_init();
	
	for (int i = 0; i < lines->tokn; i++)
	{
		// Break the line into tokens
		tokens *toks = tokenize(lines->toks[i], " ,");

		char *mnemonic = toks->toks[0];
		int i = str_to_opcode(mnemonic);
	
		printf("\n%s %d",mnemonic,i);
		assemble_fptr function = function_array[i];
	
		// If function is null then we have encountered a label
		if (function == NULL)
		{
			int32_t *address = malloc(4);
			map_put(symbol_table, mnemonic, address);

			continue;
		}

		int32_t binary_instr = function(toks->toks, symbol_table);		
		instructions[i] = binary_instr;

		//toks_free(toks);
	}

	map_free(symbol_table);
	//toks_free(lines);
	
	return instructions;
}

////  WRITE BINARY ONTO FILE  //////////////////////////////////////////////////

void write_object_code(int32_t *code, const char *path)
{
	FILE *file;
	file = fopen(path, "wb");
	fwrite(code, sizeof(int32_t), sizeof(code)/sizeof(code[0]), file);

	free(code);
}


////////////////////////////////////////////////////////////////////////////////
////  INSTRUCTION FUNCTIONS  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int32_t assemble_dataproc_result(char **tokens, map *symbol_table) { return 0; }

int32_t assemble_dataproc_mov(char **tokens, map *symbol_table) { return 1; }

int32_t assemble_dataproc_flags(char **tokens, map *symbol_table) { return 2; }

int32_t assemble_mul(char **tokens, map *symbol_table) { return 3; }

int32_t assemble_mla(char **tokens, map *symbol_table) { return 4; }

int32_t assemble_branch(char **tokens, map *symbol_table)
{
	// the following block takes the 2-char condition code mnemonic out of
	// each branch instruction mnemonic (E.g.: ble -> le -> 13)
	// base case is 'b' since b -> bal -> al -> 14	
	char *cond_code = malloc(sizeof(char*));  
	if(strcmp(tokens[0], "b") != 0) 
	{
		cond_code = tokens[0]; 
		strcpy(cond_code, cond_code + 1); // removes the 'b'
	}
	else cond_code = "al";

	char *expression = tokens[1];
	void *get = map_get(symbol_table, expression);
	int32_t address;
	if (get == NULL) address = 0; //TODO: atoi?
	else address = (int32_t) ((int32_t *) get);

	address += 8;
	int32_t offset = (((address) << 2) << 6) >> 6;
	
	//this bitfield will yield the BIG ENDIAN instruction in int32_t
	BranchInstr *instr = malloc(sizeof(BranchInstr));
	
	instr->Cond   = str_to_cond(cond_code);
	instr->_1010  = 10;
	instr->Offset = offset;

	int32_t *instr32 = (int32_t *) instr;
	print_bits(*instr32);	
	return *instr32;
}


int32_t assemble_ldr(char **tokens, map *symbol_table) { return 6; }

int32_t assemble_str(char **tokens, map *symbol_table) { return 7; }

int32_t assemble_lsl(char **tokens, map *symbol_table) { return 8; }

int32_t assemble_andeq(char **tokens, map *symbol_table) 
{ 
	return 0x00000000; 
}


