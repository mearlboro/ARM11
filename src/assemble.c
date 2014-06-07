#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "opcode_T.h"
#include "instruction_T.h"
#include "util/bitwise.h"

//////////////////////////////////////////////////////////////////////

typedef int32_t (*assemble_fptr)( char** );
assemble_fptr function_array[23]; // TODO: don't forget to change this after implementing all

int32_t assemble_dataproc_result(char **tokens) { return 0; }

int32_t assemble_dataproc_mov(char **tokens) { return 0; }

int32_t assemble_dataproc_flags(char **tokens) { return 0; }

int32_t assemble_mul(char **tokens) { return 0; }

int32_t assemble_mla(char **tokens) { return 0; }

int32_t assemble_branch(char **tokens)
{
	//TODO: change to tokens[0] when merging

	// the following block takes the 2-char condition code mnemonic out of
	// each branch instruction mnemonic (E.g.: ble -> le -> 13)
	// base case is 'b' since b -> bal -> al -> 14	
	char *cond_code = malloc(sizeof(char*));  
	if(strcmp(tokens[1], "b") != 0) 
	{
		cond_code = tokens[1]; 
		strcpy(cond_code, cond_code + 1); // removes the 'b'
	}
	else cond_code = "al";


	//this bitfield will yield the BIG ENDIAN instruction in int32_t
	BranchInstr *instr = malloc(sizeof(BranchInstr));
	
	instr->Cond   = str_to_cond(cond_code);
	instr->_1010  = 10;
	instr->Offset = 0; //TODO: symtable[tokens[1]]

	int32_t *instr32 = (int32_t *) instr;
	//print_bits(*instr32);	
	return *instr32;
}

int32_t assemble_ldr(char **tokens) { return 0; }

int32_t assemble_str(char **tokens) { return 0; }

int32_t assemble_lsl(char **tokens) { return 0; }

int32_t assemble_andeq(char **tokens) { return 0; }


void init() 
{
	int i = 0;

	for (; i < 5; i++) function_array[i] = assemble_dataproc_result;
	for (; i < 9; i++) function_array[i] = assemble_dataproc_flags;
	function_array[i++] = assemble_dataproc_mov;

	function_array[i++] = assemble_mul;
	function_array[i++] = assemble_mla;

	for (; i < 19; i++) function_array[i] = assemble_branch;

	function_array[i++] = assemble_ldr;
	function_array[i++] = assemble_str;

	function_array[i++] = assemble_lsl;
	function_array[i] = assemble_andeq;
}


void call_function(char *opcode, char **tokens) 
{
	int i = str_to_opcode(opcode);
	function_array[i](tokens);
}




int main(int argc, char **argv) 
{
	init();
	//read_assembly_program(argv[1]);
	call_function(argv[1], argv);
	

	return EXIT_SUCCESS;
}


