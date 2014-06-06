#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "tokens.h"
#include "map.h"
#include "utils.h"

//////////////////////////////////////////////////////////////  PROGAM VARIABLES

////////////////////////////////////////////////////////////////////  PROTOTYPES

tokens *read_assembly_file(const char *);

int32_t *assemble_program(tokens *);

void write_object_code(int32_t *, const char *);

//////////////////////////////////////////////////////////////////////////  MAIN

int main(int argc, char **argv)
{
	//gen_chk(argc > 2, "Please provide input and output files!");
	
	const char *path = "/Users/Zeme/ARM11/arm11_1314_testsuite/test_cases/add01.s"; //argv[1];
	
	tokens *lines = read_assembly_file(path); //toks_print(program);
	
	int32_t *code = assemble_program(lines); // argv[2];
	
	write_object_code(code, "tmp.out"); // argv[2];
	
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////  READ ASSEMBLY FILE

tokens *read_assembly_file(const char *path)
{
	FILE *file = NULL;
	
	file_chk((file = fopen(path, "rt")) != NULL, "fopen");
	file_chk(fseek(file, 0, SEEK_END)   != -1,   "fseek");
	
	long bytes = ftell(file);
	rewind(file);
	
	char buffer[bytes];
	file_chk(fread(buffer, 1, bytes, file) == bytes, "fread"); //printf("File:\n%s\n\n", buffer);
	buffer[bytes-1] = '\0';																		 // TODO WHY DO I NEED TO DO THIS???
	
	tokens *lines = tokenize(buffer, "\n");										 //toks_print(toks);
	return lines;
}

////////////////////////////////////////////////////////////////////////////////

int32_t add_instr(char **toks, map *symtbl)  /////////////////  ADD INSTRUCTION
{
	char *OpCode   = toks[0];
	char *Rd		   = toks[1];
	char *Rn		   = toks[2];
	char *Operand2 = toks[3];
	
	return 0;
}

int32_t sub_instr(char **toks, map *symtbl)  /////////////////  SUB INSTRUCTION
{
	return 0;
}

int32_t rsb_instr(char **toks, map *symtbl)  /////////////////  RSB INSTRUCTION
{
	return 0;
}

int32_t mov_instr(char **toks, map *symtbl)  /////////////////  RSB INSTRUCTION
{
	return 999999;
}

////////////////////////////////////////////////////////  ASSEMBLY FUNCTION TYPE

typedef int32_t (*assembly_function)(char **, map *);

map *create_mnemonic_to_function_map()
{
	map *m = map_new(&map_cmp_str);
	map_put(m, "add", &add_instr);
	map_put(m, "sub", &sub_instr);
	map_put(m, "rst", &rsb_instr);
	map_put(m, "mov", &mov_instr);
	// And so on...
	return m;
}

//////////////////////////////////////////////////////////  ASSEMBLE INSTRUCTION

int32_t *assemble_program(tokens *lines)
{
	// Create an array of int32_t's represeting the binary instructions
	int32_t *instructions = malloc(4 * lines->tokn);
	// Create a map mapping each mnemonic to its assembly function
	map *mne_function_map = create_mnemonic_to_function_map();
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
		assembly_function assembly_fun = map_get(mne_function_map, mnemonic);
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
	// We no longer need this map
	map_free(mne_function_map);
	// We no longer need to symbol table
	map_free(symbol_table);
	// Since we are here we may as well free lines
	toks_free(lines);
	// Returned the assebled code
	return instructions;
}

////////////////////////////////////////////////////////  WRITE BINARY ONTO FILE

void write_object_code(int32_t *code, const char *path)
{
	// TODO IMPLEMENT
	
	// We no longer need code
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
 
 */
