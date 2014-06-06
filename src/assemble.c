#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "opcode_T.h"
#include "instruction_T.h"
#include "util/bitwise.h"

//////////////////////////////////////////////////////////////////////

typedef int32_t *(*assemble_fptr)( char** );
assemble_fptr function_array[23]; // TODO: don't forget to change this after implementing all

/*
int32_t *process_and(char **tokens) 
{
	return &0;
}

int32_t *process_eor(char **tokens)
{
	return &1;
}

int32_t *process_sub(char **tokens)
{
	return &2;
}

int32_t *process_rsb(char **tokens)
{
	return &3;
}

int32_t *process_add(char **tokens)
{
	return &4;
}

int32_t *process_tst(char **tokens)
{
	return &5;
}

int32_t *process_teq(char **tokens)
{
	return &6;
}

int32_t *process_cmp(char **tokens)
{
	return &7;
}

int32_t *process_orr(char **tokens)
{
	return &8;
}

int32_t *process_mov(char **tokens)
{
	return &9;
}

int32_t *process_mul(char **tokens)
{
	return &10;
}

int32_t *process_mla(char **tokens)
{
	return &11;
}*/

int32_t *process_all_branch(char **tokens)
{
	// initialises each enum string to the 4-bit value of that condition
	set_condition_value();	
	BranchInstr *instr = malloc(sizeof(BranchInstr));

	//TODO: change to tokens[0] when merging

	// the following block takes the 2-char condition code mnemonic out of
	// each branch instruction mnemonic (E.g.: ble -> le -> 13)
	char* cond_code = tokens[1];
	if(!strcmp(cond_code, "b"))
	{ 
		cond_code[1] = 'a'; 
		cond_code[2] ='l';
	} // fixes the special case of b, since b is bal
	strcpy(cond_code, cond_code + 1); // removes the 'b'

	for (int i = 0; i < sizeof(cond_array)/sizeof(cond_array[0]); i++) 
	{
		if (strcmp(cond_code, cond_array[i].str) == 0) 
		{	
			instr->Cond = cond_array[i].cond; 
			break;
		}
	}

	//this bitfield will yield the BIG ENDIAN instruction

	instr->_1010 = 10;

	instr->Offset = 0; //TODO: symtable[tokens[1]]

	int32_t *instr32 = (int32_t *) instr;
	//print_bits(*instr32);
	//printf("%u\n", *instr32);	

	return instr32;
}
/*
int32_t *process_ldr(char **tokens)
{
	return &19;
}

int32_t *process_str(char **tokens)
{
	return &20;
}

int32_t *process_lsl(char **tokens)
{
	return &21;
}

int32_t *process_andeq(char **tokens)
{
	return &22;
}*/

void init() {
	/*function_array[0]  = process_and;
	function_array[1]  = process_eor;
	function_array[2]  = process_sub;
	function_array[3]  = process_rsb;
	function_array[4]  = process_add;
	function_array[5]  = process_tst;
	function_array[6]  = process_teq;
	function_array[7]  = process_cmp;
	function_array[8]  = process_orr;
	function_array[9]  = process_mov;
	function_array[10] = process_mul;
	function_array[11] = process_mla;*/
	for (int i = 12; i <= 18; i++)
	{ 
		function_array[i] = process_all_branch;
	}
	/*function_array[19] = process_ldr;
	function_array[20] = process_str;
	function_array[21] = process_lsl;
	function_array[22] = process_andeq;*/
}


void call_function(char *opcode, char **tokens) 
{
	for (int i = 0; i < sizeof(opcode_array)/sizeof(opcode_array[0]); i++) 
	{

		if (strcmp(opcode, opcode_array[i].str) == 0) 
		{
        		function_array[opcode_array[i].op](tokens);
		}
	}
}




int main(int argc, char **argv) 
{
	init();
	//read_assembly_program(argv[1]);
	call_function(argv[1], argv);
	

	return EXIT_SUCCESS;
}


