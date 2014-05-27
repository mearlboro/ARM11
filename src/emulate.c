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

// MASK FOR JUST THE FIRST 8 BITS - SOLVES SIGNED PROBLEM
#define EIGHT_BITS 255

////////////////////////////////////////////////////////////////////////////////
//  TYPE DEFINITIONS  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct pipeline 
{
  	int32_t fetched;			
  	int32_t decode;		
} pipeline_t;

typedef struct ARM 					 	 
{
  	int8_t memory[MEMORY_CAPACITY]; 
  	int32_t registers[REGISTER_COUNT];  
  	pipeline_t *pipeline; 				 
} ARM_t;

typedef enum 
{						                     							
  	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, 
  	SP, LR, 											 
  	PC,												     
  	CPSR													 
} register_t;

typedef enum
{
	NEGATIVE = 31,
	ZERO 	 = 30,
	CARRY 	 = 29,
	OVERFLOW = 28
} CPSR_flag_t;

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
int check_condition_code(int32_t word);
int get_CPSR_flag(CPSR_flag_t flag);
// DEBUG/TESTING
void print_ARM_info();
void system_exit(char *message); // Don't really like this...
int32_t test_glue(int8_t a, int8_t b, int8_t c, int8_t d);



////////////////////////////////////////////////////////////////////////////////
//  MAIN  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) 
{

    printf(" > Running ARM Emulator v1.0 (%s)\n", argv[0]);
    
    if (argc < 2 || argv[1] == NULL) 
        system_exit(" > Usage: emulate <input.bin>\n > Terminated\n");
  

    ARM = calloc(1, sizeof(ARM_t));
    if (ARM == NULL) system_exit(" > Fatal memory-related error");


    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) 
    	system_exit(" > Error opening input file");
    
    
    if (fseek(file, 0, SEEK_END) != 0) 
    	system_exit(" > Error reading file here");
    
    long bytes = ftell(file);
    
    rewind(file);
    

    if (fread(ARM->memory, 1, bytes, file) != bytes) 
  		system_exit(" > Error reading file lol");
 

    if (ferror(file)) 
    	system_exit(" > Error working with file");

	
    print_ARM_state();	
    // Fetch instrction at memory[0]. That is the inital value of PC
    // Load initial PC value into registers[PC]
    //ARM->registers[PC] = memory_word_read(0);
    
    //emulator_loop();
  
    fclose(file);
    free(ARM);
    return 0;
}

void system_exit(char *message)
{
	perror(message);
    exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////////////////////////
//  CORE  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void emulator_loop()
{
  ARM->pipeline->fetched = ARM->memory[ARM->registers[PC]];
  
  ARM->registers[PC] += 4;
    
  for (;;) 
  {
  	ARM->pipeline->decode = ARM->pipeline->fetched;
  
  	ARM->pipeline->fetched = ARM->memory[ARM->registers[PC]];
 
  	ARM->registers[PC] += 4;
  
  	int32_t exe_instruction = ARM->pipeline->decode;
  	
  	if (exe_instruction == 0) break; 
  	
  	if (check_condition_code(exe_instruction))
  	{
  		decode_instruction(ARM->pipeline->decode);
  	}
  }
}

void clear_CPSR_flag(CPSR_flag_t flag) 
{
	BIT_CLEAR(ARM->registers[CPSR], flag);
}

void set_CPSR_flag(CPSR_flag_t flag) 
{
	BIT_SET(ARM->registers[CPSR], flag);
}


int get_CPSR_flag(CPSR_flag_t flag) 
{
	return BIT_GET(ARM->registers[CPSR], flag);
}

void decode_instruction(int32_t word)
{  
  	int code = bits_get(word, 26, 27); 
  	switch (code) 
  	{
		case 1 : exe_single_data_transfer(word); break;
  		case 2 : exe_branch(word); break;
  		case 0 : 
  		{
  			if (BIT_GET(word, 25) == 1) exe_data_processing(word);
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



int check_condition_code(int32_t word)
{
	int cond = bits_get(word, 28, 31);
	switch (cond) 
	{
		case 0 : return get_CPSR_flag(ZERO); // eq - Z set - equal
		case 1 : return !get_CPSR_flag(ZERO); // ne - Z clear - not equal
		case 2 : return get_CPSR_flag(NEGATIVE) == get_CPSR_flag(OVERFLOW); // ge - N equals V - greater or equal
		case 3 : return get_CPSR_flag(NEGATIVE) != get_CPSR_flag(OVERFLOW);// lt - N not equal to V - less than
		case 4 : return !get_CPSR_flag(ZERO) && (get_CPSR_flag(NEGATIVE) == get_CPSR_flag(OVERFLOW)); // gt - Z clear AND (N equals V) - greater than
		case 5 : return get_CPSR_flag(ZERO) || (get_CPSR_flag(NEGATIVE) != get_CPSR_flag(OVERFLOW)); // le - Z set OR (N not equal to V) - less than or equal
		case 7 : return 1;
		default : return 0;
	
	}


}



void print_ARM_state()
{
  // Output content of registers
	
  // Oputput content of non-zero memory
  	printf("Non-zero memory:\n\n");
  	printf("Byte by Byte:\n");
  	for (uint16_t i = 0; i < MEMORY_CAPACITY; i++)
  	{
  		if (i > 10) break;
  		//int32_t word = memory_word_read(i);
  		
		printf("%i: %x\n", i, memory_byte_read(i));
		print_bits(memory_byte_read(i));
  	}
  	printf("\nWord by Word:\n");
  	for (uint16_t i = 0; i < MEMORY_CAPACITY; i += 4)
  	{
  		int32_t word = memory_word_read(i);
  		if (word == 0) break;
		printf("0x%i: %x\n", i, word);
		print_bits(word);
  	}

}


////////////////////////////////////////////////////////////////////////////////
//  INSTRUCTION EXECUTION FUNCTIONS ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void exe_single_data_transfer(int32_t word) 
{
	assert(BIT_GET(word, 26) == 1); 	

	int32_t cond = bits_get(word, 28, 31);
	int32_t Rn   = bits_get(word, 16, 19);
	int32_t Rd   = bits_get(word, 12, 15);
	int32_t off  = bits_get(word,  0, 11);

	int I = BIT_GET(word, 25);
	int P = BIT_GET(word, 24);
	int U = BIT_GET(word, 23);
	int L = BIT_GET(word, 20);

}

void exe_data_processing(int32_t word) 
{

}

void exe_multiply(int32_t word) 
{
	int Rd = bits_get(word, 16, 19);
	int Rn = bits_get(word, 12, 15);
	int Rs = bits_get(word, 8, 11);
	int Rm = bits_get(word, 0, 3);
	
	int A = BIT_GET(word, 21);
	int S = BIT_GET(word, 20);
	
	int result = ARM->registers[Rm] * ARM->registers[Rs];
	if (A == 0) 
	{
		ARM->registers[Rd] = result;
	}
	else 
	{
	    result += ARM->registers[Rn];
		ARM->registers[Rd] = result;
	}
	
	if (S == 1)
	{
		// N is set to bit 31 of the result and Z is set if and only if the result is zero.
		result_bit = BIT_GET(result, 31);
		if (result_bit == 1) 
			set_CPRS_flag(NEGATIVE);
		else 
			clear_CPRS_flag(NEGATIVE);
		
		if (result == 0) set_CPRS_flag(ZERO);
	}
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
	int32_t first  = ARM->memory[m+3]&255,
		   second = ARM->memory[m+2] & EIGHT_BITS,
		   third  = ARM->memory[m+1] & EIGHT_BITS,
		   fourth = ARM->memory[m+0] & EIGHT_BITS;
		   
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








