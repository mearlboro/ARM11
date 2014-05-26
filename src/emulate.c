#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "bitwise.c"
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
  int32_t decode;		// Current instruction/data to be decoded
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
void print_ARM_state();
int8_t memory_byte_read(uint16_t memory_address);
int32_t memory_word_read(uint16_t memory_address);
void memory_byte_write(uint16_t memory_address, int8_t byte);
void memory_word_write(uint16_t memory_address, int32_t word);
int32_t register_read(register_t reg);
void register_write(register_t reg, int32_t word);
// DEBUG/TESTING
void print_ARM_info();
void system_exit(char *message); // Don't really like this...
int32_t test_glue(int8_t a, int8_t b, int8_t c, int8_t d);



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
    
    // Seek the end of file
    if (fseek(file, 0, SEEK_END) != 0) 
    	system_exit(" > Error reading file here");
    
    long bytes = ftell(file);
    
    rewind(file);
    
    // Read binary input file (the program) and load it into ARM's main memory
    if (fread(ARM->memory, 1, bytes, file) != bytes) 
  		system_exit(" > Error reading file lol");
 
 	// Check for other errors
    if (ferror(file)) 
    	system_exit(" > Error working with file");

	
	
    // Fetch instrction at memory[0]. That is the inital value of PC
    // Load initial PC value into registers[PC]
    //ARM->registers[PC] = memory_word_read(0);
  
    
  
    //emulator_loop();
  
	print_ARM_state();
  
    // Close the file eventually
    fclose(file);
    
    // Don't forget to free up memory!
    free(ARM);


   
	//print_bits(8);
	//print_bits(bits_get(255, 0, 4)); 
	//print_bits(1);
	//print_bits(256);
	//print_bits(-256);	*/

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
  ARM->pipeline->fetched = ARM->memory[ARM->registers[PC]];
  
  // INCREMENT PROGRAM COUNTER
  ARM->registers[PC] += 4;
    
  for (;;) 
  {
  	// DECODE FETCHED INSTRUCTION
  	ARM->pipeline->decode = ARM->pipeline->fetched;
  
  	// FETCH NEXT INSTRUCTION
  	ARM->pipeline->fetched = ARM->memory[ARM->registers[PC]];
  
  	// INCREMENT PROGRAM COUNTER
  	ARM->registers[PC] += 4;
  
  	// CHECK IF WE'RE DONE
  	if (ARM->pipeline->decode == 0) break; 
  	
  	// EXECUTE CURRENT DECODED INSTRUCTION
  	decode_instruction(ARM->pipeline->decode);
  }
}

// Decode and execute the instruction contained in word 
void decode_instruction(int32_t word)
{  
  	int code = bits_get(word, 26, 27); 
  	switch (code) 
  	{
		case 1 : exe_single_data_transfer(word); break;
  		case 2 : exe_branch(word); break;
  		case 0 : 
  		{
  			if (BIT_GET(word, 25) == 1)  exe_data_processing(word);
  			else 
  			{
  				if (BIT_GET(word, 4) == 0) exe_data_processing(word);
  				else 
  				{
  					if (BIT_GET(word, 7) == 1) exe_multiply(word);
  					else exe_data_processing(word);
  				}
  			
  			}
  			break;
  		}
  		default : exit(EXIT_FAILURE); 
  	}		
  }
}

void print_ARM_state()
{
  // Output content of registers
	
  // Oputput content of non-zero memory
  	printf("Non-zero memory:\n\n");
  	printf("Byte by Byte:\n");
  	for (uint8_t i = 0; i < MEMORY_CAPACITY; i++)
  	{
  		if (i > 10) break;
  		//int32_t word = memory_word_read(i);
  		
		printf("%i: %i\n", i, memory_byte_read(i));
  	}
  	printf("\nWord by Word:\n");
  	for (uint16_t i = 0; i < MEMORY_CAPACITY; i += 4)
  	{
  		int32_t word = memory_word_read(i);
  		if (word == 0) break;
		printf("0x%i: %i\n", i, word);
  	}

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
	return ARM->memory[memory_address];
}

int32_t memory_word_read(uint16_t m) 
{
	int32_t first  = ARM->memory[m+3],
		   second = ARM->memory[m+2],
		   third  = ARM->memory[m+1],
		   fourth = ARM->memory[m+0];
		   
	return ((first << 24) | (second << 16) | (third << 8) | fourth); 
	//return ((fourth << 24) | (third << 16) | (second << 8) | first); 
}


int32_t test_glue(int8_t a, int8_t b, int8_t c, int8_t d) 
{

	return ((a << 24) + (b << 16) + (c << 8) + d); 
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








