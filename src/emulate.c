#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "bitwise.c" // Include our bit-manipulation library

////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS AND MACROS  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// ARM main memory has 2^16 = 65536 bytes
#define MEMORY_CAPACITY 65536

// ARM has 17, 32-bit registers
#define REGISTER_COUNT 17



////////////////////////////////////////////////////////////////////////////////
//  TYPE DEFINITIONS  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct pipeline // Three-stage fetch/decode/execute pipeline
{
  int32_t fetched;		// Current fetched instruction/data		
  int32_t decoded;		// Current instruction/data to be decoded
} pipeline_t;

typedef struct ARM 					 	 // ARM machine state  
{
  int8_t        memory[MEMORY_CAPACITY]; // Main memory
  int32_t    registers[REGISTER_COUNT];  // Registers
  pipeline_t *pipeline; 				 // Pipeline USE POINTER INSTEAD?
} ARM_t;

typedef enum // Mnemonics for referencing registers in ARM.registers[register_t]
{						                     							
  R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, // General purpose
  __SP, __LR, 											 // Unused, pls ignore
  PC,												     // Program Counter
  CPSR													 // CPSR, 'nuff said
} register_t;



////////////////////////////////////////////////////////////////////////////////
//  ARM OBJECT  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ARM_t *ARM = NULL;



////////////////////////////////////////////////////////////////////////////////
//  FUNCTION PROTOTYPES  ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void emulator_loop();
void decode_instruction(int32_t word);
void exe_single_data_transfer(int32_t word);
void exe_data_processing(int32_t word);
void exe_multiply(int32_t word);
void exe_branch(int32_t word);
void optput_ARM_state();
int8_t memory_byte_read(uint16_t memory_address);
int32_t memory_word_read(uint16_t memory_address);
void memory_byte_write(uint16_t memory_address, int8_t byte);
void memory_word_write(uint16_t memory_address, int32_t word);
int32_t register_read(register_t reg);
void register_write(register_t reg, int32_t word);
// DEBUG/TESTING
void print_ARM_info();
void system_exit(char *message); // Don't really like this...



////////////////////////////////////////////////////////////////////////////////
//  MAIN  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) 
{
    // Welcome the user and perform sanity check
    printf(" > Running ARM Emulator v1.0 (%s)\n", argv[0]);
    if (argc < 2 || argv[1] == NULL) 
        system_exit(" > Usage: emulate <input.bin>\n > Terminated\n");
  
    // Now that file is open, initialize our ARM object
    ARM = calloc(1, sizeof(ARM_t));
    if (ARM == NULL) system_exit(" > Fatal memory-related error");

    // Open the binary input file
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) 
    	system_exit(" > Error opening input file");
    
    
    
    
    // Read binary input file (the program) and load it into ARM's main memory
    if (fread(ARM->memory, fseek, 1, file)  
  		system_exit(" > Error reading file");
 
 	// Check for other errors
    if (ferror(file)) 
    	system_exit(" > Error working with file");

    
    fseek
    // Fetch instrction at memory[0]. That is the inital value of PC
  
    // Load initial PC value into registers[PC]
  
    // Begin emulator_loop()
    printf(" > test = %i\n", test);
  
    // Close the file eventually
    fclose(file);
    
    // Don't forget to free up memory!
    free(ARM);
   
	

}

void system_exit(char *message)
{
	perror(message);
    exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////////////////////////
//  CORE  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Simulates the three-stage simulation pipeline
void emulator_loop()
{
  // FETCH NEXT INSTRUCTION
  // pipeline fetched <-- [PC]
  
  // INCREMENT PROGRAM COUNTER
  // PC = PC + 4
  
  
  /*** main_loop ***/
  // DECODE FETCHED INSTRUCTION
  // pipeline decoded <-- pipeline fetched
  
  // FETCH NEXT INSTRUCTION
  // pipeline fetched <-- [PC]
  
  // INCREMENT PROGRAM COUNTER
  // PC = PC + 4
  
  // EXECUTE CURRENT DECODED INSTRUCTION
  // execute_instruction(pipeline decoded)
  /*** goto main_loop ***/
 
}

// Decode and execute the instruction contained in word 
void decode_instruction(int32_t word)
{  
  // Check instruction (word)
  // IF: is HALT (32 zeros) THEN terminate and output_ARM_state()
  // ELSE execute it
  
  /*
  
  word[n] means the nth bit of word
  
  ########################################################
  consider word[27] and word[26]
  
  if      01 then exe_single_data_transfer(word)
  
  else if 10 then exe_branch(word)
  
  else if 00 then {
  	
  	if      word[25] == 1 then exe_data_processing(word [operand2 is a shifted register])
  	 
  	else if word[25] == 0 then {
  	
  	  if      word[4] == 0 then exe_data_processing(word)
  	  
  	  else if word[4] == 1 then {
    	  
    	if      word[7] == 1 then exe_multiply(word)
    	
    	else if word[7] == 0 then exe_data_processing(word)
  	  
  	  }
  	  
  	} 
  		
  }
  	
  else ERROR_ERROR_EXTERMINATE_EXTERMINATE!!!
  ########################################################
   
   */
}

void optput_ARM_state()
{
  // Output content of registers
	
  // Oputput content of non-zero memory
}



////////////////////////////////////////////////////////////////////////////////
//  INSTRUCTION EXECUTION FUNCTIONS ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void exe_single_data_transfer(int32_t word) 
{

}

void exe_data_processing(int32_t word) 
{

}

void exe_multiply(int32_t word) 
{

}

void exe_branch(int32_t word) 
{

}



////////////////////////////////////////////////////////////////////////////////
//  MEMORY/REGISTERS READ/WRITE ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int8_t memory_byte_read(uint16_t memory_address) 
{
	return 0;
}

int32_t memory_word_read(uint16_t memory_address) 
{
	return 0;
}

void memory_byte_write(uint16_t memory_address, int8_t byte) 
{

}

void memory_word_write(uint16_t memory_address, int32_t word) 
{

}

int32_t register_read(register_t reg) 
{
	return 0;
}

void register_write(register_t reg, int32_t word) 
{

}



////////////////////////////////////////////////////////////////////////////////
//  TESTING/DEBUG  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void print_ARM_info() 
{
	// In ARM_t, memory array is 65536 bytes, registers array is 17*4=68 bytes.
	// 65536 + 68 = 65604 bytes.
	// sizeof(ARM_t) returns 65612.
	// The compiled padded the ARM struct to achieve better performance, 
	// hence the mysterious extra 8 bytes... Thanks compiler!  
	//freopen("data.in","r",stdin);
	//freopen("data.out","w",stdout);
	//printf("struct ARM is %lu bytes.\n", sizeof(ARM_t));
	
	// sizeof(ARM) returns 8 bytes, the size of a pointer, since ARM is a
	// pointer variable (duh). sizeof(ARM_t *) also returns 8
	//printf("\"ARM_t ARM = NULL;\" sizeof(ARM) = sizeof(ARM_t *) = %lu = %lu bytes.\n", sizeof(ARM), sizeof(ARM_t*));
	
	//ARM_t ARM_test;
	//printf("\"ARM_t ARM_test;\" sizeof(ARM_test) = %lu bytes\n", sizeof(ARM_test));
	
	
	printf("sizeof(*ARM) = %lu bytes.\n", sizeof(*ARM));
	
	
}








