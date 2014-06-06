#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "util/tokens.h"
#include "util/map.h"
#include "util/utils.h"


#include "opcode_T.h"

//////////////////////////////////////////////////////////////////////

typedef int32_t (*assemble_fptr)( char**, map* );
assemble_fptr function_array[23];

int32_t process_and(char **tokens, map *symtable) 
{
	
	return 0;
}

int32_t process_eor(char **tokens, map *symtable)
{
	return 1;
}

int32_t process_sub(char **tokens, map *symtable)
{
	return 2;
}

int32_t process_rsb(char **tokens, map *symtable)
{
	return 3;
}

int32_t process_add(char **tokens, map *symtable)
{
	return 4;
}

int32_t process_tst(char **tokens, map *symtable)
{
	return 5;
}

int32_t process_teq(char **tokens, map *symtable)
{
	return 6;
}

int32_t process_cmp(char **tokens, map *symtable)
{
	return 7;
}

int32_t process_orr(char **tokens, map *symtable)
{
	return 8;
}

int32_t process_mov(char **tokens, map *symtable)
{
	return 9;
}

int32_t process_mul(char **tokens, map *symtable)
{
	/*for (int i=0; i<tokno; i++)
	{
		
	} */
	return 10;
}

int32_t process_mla(char **tokens, map *symtable)
{
	return 11;
}

int32_t process_beq(char **tokens, map *symtable)
{
	return 12;
}

int32_t process_bne(char **tokens, map *symtable)
{
	return 13;
}

int32_t process_bge(char **tokens, map *symtable)
{
	return 14;
}

int32_t process_blt(char **tokens, map *symtable)
{
	return 15;
}

int32_t process_bgt(char **tokens, map *symtable)
{
	return 16;
}

int32_t process_ble(char **tokens, map *symtable)
{
	return 17;
}

int32_t process_b(char **tokens, map *symtable)
{
	return 18;
}

int32_t process_ldr(char **tokens, map *symtable)
{
	return 19;
}

int32_t process_str(char **tokens, map *symtable)
{
	return 20;
}

int32_t process_lsl(char **tokens, map *symtable)
{
	return 21;
}

int32_t process_andeq(char **tokens, map *symtable)
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

#define not !

assemble_fptr assign_function (char *opcode, char **tokens) 
{
	for (int i=0; i < sizeof(opcode_array)/sizeof(opcode_array[0]); i++) 
	{
		char *to_compare = opcode_array[i].str;
		if (strcmp(opcode, to_compare) == 0) 
		{
			//result = (int32_t *) &function_array[opcode_array[i].op];
			return function_array[opcode_array[i].op];
		}
		
	}

	return NULL;
}
	
//////////////////  PROGAM VARIABLES  //////////////////////////////////////////

//////////////////  PROTOTYPES  ////////////////////////////////////////////////

tokens *read_assembly_file(const char *);

int32_t *assemble_program(tokens *);

void write_object_code(int32_t *, const char *);

//////////////////  MAIN  //////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	//gen_chk(argc > 2, "Please provide input and output files!");
	init();	

	//const char *path = "/Users/Zeme/ARM11/arm11_1314_testsuite/test_cases/add01.s"; //argv[1];
	
	tokens *lines = read_assembly_file(argv[1]); //toks_print(program);
	
	int32_t *code = assemble_program(lines);  // argv[2];
	
	write_object_code(code, argv[2]);       // argv[2];

	return EXIT_SUCCESS;
}

//////////////////  READ ASSEMBLY FILE  ////////////////////////////////////////

tokens *read_assembly_file(const char *path)
{
	FILE *file = NULL;
	
	file_chk((file = fopen(path, "rt")) != NULL, "fopen");
	file_chk(fseek(file, 0, SEEK_END)   != -1,   "fseek");
	
	long bytes = ftell(file);
	rewind(file);
	
	char buffer[bytes];
	//printf("File:\n%s\n\n", buffer);
	file_chk(fread(buffer, 1, bytes, file) == bytes, "fread"); 
	// TODO WHY DO I NEED TO DO THIS???
	buffer[bytes-1] = '\0';                                    
	tokens *lines = tokenize(buffer, "\n"); //toks_print(toks);
	return lines;
}


int32_t *assemble_program(tokens *lines)
{
	// Create an array of int32_t's represeting the binary instructions decoded
	int32_t *instructions = malloc(4 * lines->tokn);
	// Create a sybol table for storing labels and addresses
	map *symbol_table = map_new(&map_cmp_str);
	// For each assembly line in the program
	for (int i = 0; i < lines->tokn; i++)
	{
		// Break the line into tokens
		tokens *toks = tokenize(lines->toks[i], " ,"); toks_print(toks);
		// Obtain the mnemonic, always the first token
		char *mnemonic = toks->toks[0];
		// Obtain assmbly function from the mnemonic string
		assemble_fptr assembly_fun = assign_function(mnemonic, toks->toks);
		// If assembly_fun is null then we have encountered a label
		if (assembly_fun == NULL)
		{
			// Compute the machine address and store it on the heap
			int32_t *address = malloc(4);
			// The address is the current line number * 4 byte (since 32-bit words)
			*address = 4 * i;
			// Insert the new label into the symbol table
			map_put(symbol_table, mnemonic, address);
			// Now simply continue to the next instruction
			continue;
		}
		// Invoke function and onbtain a new binary instruction
		int32_t binary_instr = assembly_fun(toks->toks, symbol_table);		
		// Store the binary instruction into the array
		instructions[i] = binary_instr;
		// We no longer need these tokens
		toks_free(toks);
	}
	// We no longer need to symbol table
	map_free(symbol_table);
	// Since we are here we may as well free lines
	toks_free(lines);
	// Return the assembled code
	return instructions;
}

////////////////////////////////////////////////////////  WRITE BINARY ONTO FILE

void write_object_code(int32_t *code, const char *path)
{
	FILE *file;
	file=fopen(path, "wb");
	fwrite(code, sizeof(int32_t), sizeof(code)/sizeof(code[0]), file);

	free(code);
}

////////////////////////////////////////////////////////////////////////////////



/*
 add Add Rd := Rn + Op2 Data Processing
 sub Subtract Rd := Rn - Op2 Data Processing
 rsb Reverse Subtract Rd := Op2 - Rn Data Processing
 and AND Rd := Rn AND Op2 Data Processing
 eor Exclusive OR Rd := Rn XOR Op2 Data Processing
 orr OR Rd := Rn OR Op2 Data Processing
 mov Move Assignment Rd := Op2 Data Processing
 tst Test bits CPSR ﬂags := Rn AND Op2 Data Processing
 teq Test bitwise equality CPSR ﬂags := Rn XOR Op2 Data Processing
 cmp Compare CPSR ﬂags := Rn - Op2 Data Processing
 */

// LECTURE 5 FROM TOP TO BOTTOM
// GLOBAL VARIABLES AVOID!!!
// COMMENT EVERYTHING!!!
// MAKE SURE YOU HAVE PROTOTYPES IN .h FILE AND INCLUDE IT


/*
 
 int main(int argc, char **argv) {
 
 
 ListNode * head = malloc(sizeof(ListNode));
 head->data = 4;
 head->next = NULL;
 head=list_insert(head,13);
 list_print(head);
 head=list_insert(head,4);
 list_print(head);
 head=list_insert(head,6);
 list_print(head);
 head=list_insert(head,10);
 list_print(head);
 head=list_remove(head,6);
 list_print(head);
 head=list_remove(head,4);
 printf("%d", (list_search(head, 4) == NULL) ? 0 : 1);
 
 SymbolTable *new = symtable_init();
 
 symtable_put(new,"lala",0x99);
 symtable_print(new);
 symtable_remove(new, "lala");
 symtable_print(new);
 
 return EXIT_SUCCESS;
 }
 
 Tokens *toks_new()
 {
 Tokens *tokens = NULL;
 ERR_MEM((tokens       = malloc(sizeof(Tokens)))  == NULL);
 ERR_MEM((tokens->toks = malloc(sizeof(char **))) == NULL);
 return tokens;
 }
 
 
 Tokens *read_assembly_program(const char *filepath)
 {
 FILE *file = fopen(filepath, "rt");
 
 ERR_FILE(file == NULL);
 ERR_FILE(fseek(file, 0, SEEK_END) == -1);
 
 long bytes = ftell(file);
 rewind(file);
 
 char buffer[bytes];	// Contains the whole assembly text
 ERR_FILE(fread(buffer, 1, bytes, file) != bytes);
 
 buffer[bytes-1] = '\0'; // Without this we get an indecipherable line at the end... But y?
 
 printf("%s\n", buffer);
 return tokenize(buffer, "\n");
 
 //return to
 //toks_print(program); // DEBUG ONLY
 }
 
 ///////////////////////////////////////////////////////////////  DATA PROCESSING
 
 int32_t ass_data_processing_instr(tokens *toks)
 {
 
 return 0;
 }
 
 //////////////////////////////////////////////////////////  SINGLE DATA TRANSFER
 
 int32_t ass_single_data_transfer_instr(tokens *toks)
 {
 
 return 0;
 }
 
 //////////////////////////////////////////////////////////////////////  MULTIPLY
 
 int32_t ass_multiply_instr(tokens *toks)
 {
 
 return 0;
 }
 
 ////////////////////////////////////////////////////////////////////////  BRANCH
 
 int32_t ass_branch_instr(tokens *toks)
 {
 
 return 0;
 }
 
 ///////////////////////////////////////////////////////////////////////  SPECIAL
 
 int32_t ass_special_instr(tokens *toks)
 {
 
 return 0;
 }
 
 ////////////////////////////////////////////////////////////////////////////////
 static ass_fun ass_functions[] =
 {
 &ass_data_processing_instr,
 &ass_single_data_transfer_instr,
 &ass_multiply_instr,
 &ass_branch_instr,
 &ass_special_instr
 };
 
 static char *mnemonics[] =
 {
 "add",
 "sub",
 "rsb",
 "and",
 "eor",
 "orr",
 "mov",
 "tst",
 "teq",
 "cmp",
 "mul",
 "mla",
 "ldr",
 "str",
 "beq",
 "bne",
 "bge",
 "blt",
 "bgt",
 "ble",
 "b",
 "lsl",
 "andeq"
 };
 char *mnemonics[] =
 {
 "add",
 "sub",
 "rsb",
 "and",
 "eor",
 "orr",
 "mov",
 "tst",
 "teq",
 "cmp",
 "mul",
 "mla",
 "ldr",
 "str",
 "beq",
 "bne",
 "bge",
 "blt",
 "bgt",
 "ble",
 "b",
 "lsl",
 "andeq"
 };
 
 for (int i = 0; i < 23; i++)
 {
 printf("int32_t %s_instr(char **toks, map *symtbl)  /////////////////  %s INSTRUCTION\n{\n\treturn %i;\n}\n\n", mnemonics[i], mnemonics[i], i);
 }
 */
