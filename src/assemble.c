#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "tokens.h"
#include "map.h"
#include "utils.h"
#include "instruction_T.h"
#include "bitwise.h"

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
	
	int32_t *code = assemble_program(lines);  // argv[2];
	
	write_object_code(code, "tmp.out");       // argv[2];

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

int as_immediate_const(char *Operand2)
{
	unsigned long value;
	if   (strstr(Operand2, "0x")) value = strtoul(Operand2, 0, 16);     // Hexa
	else												  value = atoi(Operand2 + 1);           // Decimal
	// TODO inverse-rotate n.o.t e.a.s.y
	return 0;
}

int is_immediate_const(char *Operand2) { return strstr(Operand2, "#") != NULL; }

int  as_shift_register(char *Operand2) { return -1; }        // Optional for now

int dpi_mne_to_opcode(char *mnemonic)  { return 0; }           // TODO implement

int parse_Operand2(char *Operand2)
{
	return (is_immediate_const(Operand2)) ? as_immediate_const(Operand2)
	                                      : as_shift_register(Operand2);
}


int32_t assemble_dpi(char **toks, int S, int Rn, int Rd, int Operand2)
{
	// [Cond 00 I OpCode S Rn   Rd   Operand2    ]
	DataProcessingInstr instr;
	
	instr.Cond     = AL;
	instr._00			 = 0;
	instr._I			 = is_immediate_const(toks[Operand2]);
	instr.OpCode   = dpi_mne_to_opcode(toks[0]);
	instr.S				 = S;
	instr.Rn			 = (Rn == -1) ? 0 : atoi(toks[Rn]+1);
	instr.Rd			 = (Rd == -1) ? 0 : atoi(toks[Rd]+1);
	instr.Operand2 = parse_Operand2(toks[Operand2]);
	
	return *((int32_t *) &instr);
}

int32_t ass_dpi_result(char **toks, map *symtbl) // and, eor, sub, rsb, add, orr
{
	// Syntax: <opcode> Rd, Rn, <Operand2>
	return assemble_dpi(toks, 0, 2, 1, 3);
}

int32_t ass_dpi_mov(char **toks, map *symtbl) // mov
{
	// Syntax: mov Rd, <Operand2> { Rn ignored }
	return assemble_dpi(toks, 0, -1, 1, 2);
}

int32_t ass_dpi_cprs(char **toks, map *symtbl)// tst, teq, cmp
{
	// Syntax: <opcode> Rn, <Operand2> { Rd ignored }
	return assemble_dpi(toks, 1, 1, -1, 2);
}


////////////////////////////////////////////////////////////////////////////////

int32_t assemble_mul(char **toks, int A, int Rd, int Rm, int Rs, int Rn)
{
	// [Cond 000000 A S Rd   Rn   Rs   1001 Rm  ]
	MultiplyInstr instr;
	
	instr.Cond    = AL;
	instr._000000	= 0;
	instr.A			  = A;
	instr.S			  = 0;
	instr.Rd			= atoi(toks[Rd]+1);
	instr.Rn			= (Rn == -1) ? 0 : atoi(toks[Rn]+1);
	instr.Rs			= atoi(toks[Rs]+1);
	instr._1001   = 9;
	instr.Rm			= atoi(toks[Rm]+1);
	
	return *((int32_t *) &instr);
}

int32_t ass_mul(char **toks, map *symtbl) // mul
{
	// Syntax: mul Rd, Rm, Rs { Rn ignored }
	return assemble_mul(toks, 0, 1, 2, 3, -1);
}

int32_t ass_mla(char **toks, map *symtbl) // mla
{
	// Syntax: mla Rd, Rm, Rs, Rn
	return assemble_mul(toks, 1, 1, 2, 3, 4);
}


////////////////////////////////////////////////////////////////////////////////

int32_t assemble_sdt(char **toks, map *symtbl)
{
	// [Cond 01 I P U 0 0 L Rn   Rd   Offset   ]
	SingleDataTransferInstr instr;

	
	return *((int32_t *) &instr);
}


////////////////////////////////////////////////////////////////////////////////

int32_t andeq_instr(char **toks, map *symtbl)  // andeq
{
	return 0x00000000;
}

////////////////////////////////////////////////////////////////////////////////

int32_t lsl_instr(char **toks, map *symtbl)  // lsl 
{
	return 21;
}

////////////////////////////////////////////////////////////////////////////////

int32_t ldr_instr(char **toks, map *symtbl)  //////////////////  ldr INSTRUCTION
{
	// Syntax: ldr Rd, address
	// L = 1;
	return 12;
}

int32_t str_instr(char **toks, map *symtbl)  //////////////////  str INSTRUCTION
{
	return 13;
}

int32_t beq_instr(char **toks, map *symtbl)  //////////////////  beq INSTRUCTION
{
	return 14;
}

int32_t bne_instr(char **toks, map *symtbl)  //////////////////  bne INSTRUCTION
{
	return 15;
}

int32_t bge_instr(char **toks, map *symtbl)  //////////////////  bge INSTRUCTION
{
	return 16;
}

int32_t blt_instr(char **toks, map *symtbl)  //////////////////  blt INSTRUCTION
{
	return 17;
}

int32_t bgt_instr(char **toks, map *symtbl)  //////////////////  bgt INSTRUCTION
{
	return 18;
}

int32_t ble_instr(char **toks, map *symtbl)  //////////////////  ble INSTRUCTION
{
	return 19;
}

int32_t b_instr(char **toks, map *symtbl)    /////////////////     b INSTRUCTION
{
	return 20;
}



////////////////////////////////////////////////////////  ASSEMBLY FUNCTION TYPE

typedef int32_t (*assembly_function)(char **, map *);

map *create_mnemonic_to_function_map()
{
	map *m = map_new(&map_cmp_str);
	map_put(m, "add", &ass_dpi_result);
	map_put(m, "sub", &ass_dpi_result);
	map_put(m, "rsb", &ass_dpi_result);
	map_put(m, "and", &ass_dpi_result);
	map_put(m, "eor", &ass_dpi_result);
	map_put(m, "orr", &ass_dpi_result);
	map_put(m, "mov", &ass_dpi_mov);
	map_put(m, "tst", &ass_dpi_cprs);
	map_put(m, "teq", &ass_dpi_cprs);
	map_put(m, "cmp", &ass_dpi_cprs);
	map_put(m, "mul", &ass_mul);
	map_put(m, "mla", &ass_mla);
	map_put(m, "ldr", &ldr_instr);
	map_put(m, "str", &str_instr);
	map_put(m, "beq", &beq_instr);
	map_put(m, "bne", &bne_instr);
	map_put(m, "bge", &bge_instr);
	map_put(m, "blt", &blt_instr);    // Becon Lettice Tomato
	map_put(m, "bgt", &bgt_instr);
	map_put(m, "ble", &ble_instr);
	map_put(m, "b"  , &b_instr);
	map_put(m, "lsl", &lsl_instr);
	map_put(m, "andeq", &andeq_instr); // OCD
	return m;
}

//////////////////////////////////////////////////////////////  ASSEMBLE PROGRAM

int32_t *assemble_program(tokens *lines)
{
	// Create an array of int32_t's represeting the binary instructions decoded
	int32_t *instructions = malloc(4 * lines->tokn);
	// Create a map, mapping each mnemonic to its assembly function
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
		int32_t binary_instr = assembly_fun(toks->toks, symbol_table); print_bits(binary_instr);
		// Store the binary instruction into the array
		instructions[i] = binary_instr;
		// We no longer need these tokens
		toks_free(toks);
	}
	// We no longer need this map
	map_free(mne_function_map, MAP_FREE_NON);
	// We no longer need to symbol table
	map_free(symbol_table, MAP_FREE_VAL);
	// Since we are here we may as well free lines
	toks_free(lines);
	// Return the assembled code
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
 
 static map *mnemonic_to_opcode()
 {
 map *m = map_new(&map_cmp_str);
 map_put(m, "and", heap_int(0));
 map_put(m, "eor", heap_int(1));
 map_put(m, "sub", heap_int(2));
 map_put(m, "rsb", heap_int(3));
 map_put(m, "add", heap_int(4));
 map_put(m, "orr", heap_int(12));
 map_put(m, "mov", heap_int(13));
 map_put(m, "tst", heap_int(8));
 map_put(m, "teq", heap_int(9));
 map_put(m, "cmp", heap_int(10));
 return m;
 }*/

