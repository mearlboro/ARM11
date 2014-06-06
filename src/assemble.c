#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "opcode_T.h"

//////////////////////////////////////////////////////////////////////

typedef int32_t (*assemble_fp)( char** );
assemble_fp function_array[23];

int32_t process_and(char **tokens) 
{
	
	return 0;
}

int32_t process_eor(char **tokens)
{
	return 1;
}

int32_t process_sub(char **tokens)
{
	return 2;
}

int32_t process_rsb(char **tokens)
{
	return 3;
}

int32_t process_add(char **tokens)
{
	return 4;
}

int32_t process_tst(char **tokens)
{
	return 5;
}

int32_t process_teq(char **tokens)
{
	return 6;
}

int32_t process_cmp(char **tokens)
{
	return 7;
}

int32_t process_orr(char **tokens)
{
	return 8;
}

int32_t process_mov(char **tokens)
{
	return 9;
}

int32_t process_mul(char **tokens)
{
	/*for (int i=0; i<tokno; i++)
	{
		
	} */
	return 10;
}

int32_t process_mla(char **tokens)
{
	return 11;
}

int32_t process_beq(char **tokens)
{
	return 12;
}

int32_t process_bne(char **tokens)
{
	return 13;
}

int32_t process_bge(char **tokens)
{
	return 14;
}

int32_t process_blt(char **tokens)
{
	return 15;
}

int32_t process_bgt(char **tokens)
{
	return 16;
}

int32_t process_ble(char **tokens)
{
	return 17;
}

int32_t process_b(char **tokens)
{
	return 18;
}

int32_t process_ldr(char **tokens)
{
	return 19;
}

int32_t process_str(char **tokens)
{
	return 20;
}

int32_t process_lsl(char **tokens)
{
	return 21;
}

int32_t process_andeq(char **tokens)
{
	return 22;
}

void init() {
	function_array[0]  = process_and;
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
	function_array[11] = process_mla;
	function_array[12] = process_beq;
	function_array[13] = process_bne;
	function_array[14] = process_bge;
	function_array[15] = process_blt;
	function_array[16] = process_bgt;
	function_array[17] = process_ble;
	function_array[18] = process_b;
	function_array[19] = process_ldr;
	function_array[20] = process_str;
	function_array[21] = process_lsl;
	function_array[22] = process_andeq;
}


void call_function(char *opcode, char **tokens) 
{
	for (int i=0; i < sizeof(opcode_array)/sizeof(opcode_array[0]); i++) 
	{
		char *to_compare = opcode_array[i].str;
        	if (strcmp(opcode, to_compare) == 0) 
		{
        		printf("%d",function_array[opcode_array[i].op](tokens));
		}
	}
}


void write_binary_file (const char *filename, void *command)
{
	FILE *file;
	file=fopen(filename, "wb");
	fwrite(command, sizeof(int32_t), 1, file);
}


int main(int argc, char **argv) 
{
	init();
	//read_assembly_program(argv[1]);
	call_function(argv[1], argv);
	

	return EXIT_SUCCESS;
}


