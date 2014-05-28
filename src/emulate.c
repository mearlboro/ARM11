#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "bitwise.h"

////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS AND MACROS  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// ARM main memory has 2^16 = 65536 bytes
#define MEMORY_CAPACITY 65536

// ARM has 17, 32-bit registers
#define REGISTER_COUNT 17

// mask for the first 8 bits
#define EIGHT_BITS 255


////////////////////////////////////////////////////////////////////////////////
//  INSTRUCTION TYPE DEFINITIONS ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// data processing istructions
typedef struct DATA_PROCESSING_INSTRUCTION
{
	unsigned int Operand2 : 12;
	unsigned int Rd       : 4;
	unsigned int Rn       : 4;
	unsigned int S        : 1;
	unsigned int OpCode   : 4;
	unsigned int I        : 1;
	unsigned int _00      : 2;
	unsigned int Cond     : 4;
} DATA_PROCESSING_INSTRUCTION;


// for operand2 and offset in DATA_PROCESSING_INSTRUCTION
// and SINGLE_DATA_TRANSFER_INSTRUCTION

typedef struct IMMEDIATE_REGISTER
{
	unsigned int Rotate : 4;
	unsigned int Imm    : 8;

} IMMEDIATE_REGISTER; // rotates the immediate register


typedef struct SHIFT_REGISTER
{
	unsigned int Shift_amount : 5;
	unsigned int Shift_type   : 2;
	unsigned int Shift_flag   : 1;
	unsigned int Rm           : 4;

} SHIFT_REGISTER; // performs only shift by constant amount


// multiply instruction
typedef struct MULTIPLY_INSTRUCTION
{
    unsigned int Cond    : 4;
    unsigned int _000000 : 6;
    unsigned int A       : 1;
    unsigned int S       : 1;
    unsigned int Rd      : 4;
    unsigned int Rn      : 4;
    unsigned int Rs      : 4;
    unsigned int _1001   : 4;
    unsigned int Rm      : 4;

} MULTIPLY_INSTRUCTION;


// single data transfer instruction
typedef struct SINGLE_DATA_TRANSFER_INSTRUCTION
{
    unsigned int Cond   : 4;
    unsigned int _01    : 2;
    unsigned int I      : 1;
    unsigned int P      : 1;
    unsigned int U      : 1;
    unsigned int _00    : 2;
    unsigned int L      : 1;
    unsigned int Rn     : 4;
    unsigned int Rd     : 4;
    unsigned int Offset : 12;

} SINGLE_DATA_TRANSFER_INSTRUCTION;


// branch instruction
typedef struct BRANCH_INSTRUCTION
{
    unsigned int Cond   : 4;
    unsigned int _1010  : 4;
    unsigned int Offset : 24;

} BRANCH_INSTRUCTION;

////////////////////////////////////////////////////////////////////////////////
//  OTHER TYPE DEFINITIONS /////////////////////////////////////////////////////
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
} registerT;

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
int32_t register_read(registerT reg);
void register_write(registerT reg, int32_t word);
int check_condition_code(int32_t word);
int get_CPSR_flag(CPSR_flag_t flag);

// DEBUG/TESTING
void print_ARM_info();
void system_exit(char *message);
int32_t test_glue(int8_t a, int8_t b, int8_t c, int8_t d);

// OTHER HELPER FUNCTIONS
int32_t immediate_shifted_register(int32_t word12, int8_t S);


////////////////////////////////////////////////////////////////////////////////
//  MAIN  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    printf(" > Running ARM Emulator v1.0 (%s)\n", argv[0]);

    if (argc < 2 || argv[1] == NULL)
        system_exit(" > Usage: emulate <input.bin>\n > Terminated\n");


    ARM = calloc(1, sizeof(ARM_t));
    ARM->pipeline = calloc(1, sizeof(pipeline_t));
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

    // print_ARM_state();
    // Fetch instrction at memory[0]. That is the inital value of PC
    // Load initial PC value into registers[PC]
    ARM->registers[PC] = 0;
    print_ARM_state();
    emulator_loop();

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

// pipeline simulation
void emulator_loop()
{
  int32_t reg = memory_word_read(ARM->registers[PC]);

  ARM->pipeline->fetched = reg;

  ARM->registers[PC] += 4;

  for (;;)
  {
  	ARM->pipeline->decode = ARM->pipeline->fetched;

  	ARM->pipeline->fetched = memory_word_read(ARM->registers[PC]);

  	ARM->registers[PC] += 4;

  	int32_t exe_instruction = ARM->pipeline->decode;

  	if (exe_instruction == 0) break;

  	if (check_condition_code(exe_instruction))
  	{
  		decode_instruction(ARM->pipeline->decode);
  	}
  }
}


// flag operations
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

void put_CPSR_flag(CPSR_flag_t flag, int bit)
{
  if (bit == 1) set_CPSR_flag(flag);
  else clear_CPSR_flag(flag);
}


// decode 32-bit instruction
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


// checks the 4-bit conditions at the beginning of instruction
int check_condition_code(int32_t word)
{
	int32_t cond = bits_get(word, 28, 31);
	switch (cond)
	{
		case 0  : return get_CPSR_flag(ZERO);
			// eq - Z set - equal
		case 1  : return !get_CPSR_flag(ZERO);
			// ne - Z clear - not equal
		case -6 : return get_CPSR_flag(NEGATIVE) == get_CPSR_flag(OVERFLOW);
			// ge - N equals V - greater or equal
		case -5 : return get_CPSR_flag(NEGATIVE) != get_CPSR_flag(OVERFLOW);
			// lt - N not equal to V - less than
		case -4 : return !get_CPSR_flag(ZERO) &&
				(get_CPSR_flag(NEGATIVE) == get_CPSR_flag(OVERFLOW));
			// gt - Z clear AND (N equals V) - greater than
		case -3 : return get_CPSR_flag(ZERO) ||
				(get_CPSR_flag(NEGATIVE) != get_CPSR_flag(OVERFLOW));
			// le - Z set OR (N not equal to V) - less than or equal
		case -2 : return 1;
		default : return 0;
	}

}


// outputs content of registers and non-zero memory
void print_ARM_state()
{

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
		printf("0x%i: %i\n", i, word);
		print_bits(word);
  	}

  	printf("\nREGISTERS:\n");
  	for (int i = 0; i < REGISTER_COUNT; i++)
    {
        printf("#%2i: %i\n",i,ARM->registers[i]);
    }
}


////////////////////////////////////////////////////////////////////////////////
//  INSTRUCTION EXECUTION FUNCTIONS ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void exe_single_data_transfer(int32_t word)
{

	assert(bits_get(word, 26, 27) == 2);
	assert(check_condition_code(word));

	int32_t Rn  = bits_get(word, 16, 19);
	int32_t Rd  = bits_get(word, 12, 15);
	int32_t offset = bits_get(word,  0, 11);
	int I = BIT_GET(word, 25);
	int P = BIT_GET(word, 24);
	int U = BIT_GET(word, 23);
	int L = BIT_GET(word, 20);

	if (I == 0)
	{
		int32_t Imm = ZERO_EXT_32(bits_get(offset, 0, 7));
		int8_t rotate_amount = bits_get(offset, 8, 11) << 1;
		Imm = rotate(Imm, rotate_amount);
		offset = Imm;
	}
	else
	{
		offset = immediate_shifted_register(offset, 0);
	}


	if (L == 1) // Loading from memory into Rd
	{
		ARM->registers[Rd] = ARM->memory[ARM->registers[Rn]];
	}
	else
	{
		ARM->memory[ARM->registers[Rn]] = ARM->registers[Rd];
	}

	if (P == 0) // Post-indexing
	{
		if (U == 0)
		{
			ARM->registers[Rn] -= offset;
		}
		else
		{
			ARM->registers[Rn] += offset;
		}
	}
}



void exe_data_processing(int32_t word)
{
	/*assert(!BIT_GET(word, 25) && !BIT_GET(word, 26)
		&& !(BIT_GET(word, 4) && BIT_GET(word, 7)));
 	assert(check_condition_code(word));*/

	DATA_PROCESSING_INSTRUCTION *instruction
		= (DATA_PROCESSING_INSTRUCTION *) &word;

	int32_t I        = instruction->I;
	int32_t opCode   = instruction->OpCode;
	int32_t S        = instruction->S;
	int32_t Rn       = instruction->Rn;
	int32_t Rd       = instruction->Rd;
	int32_t operand2 = instruction->Operand2;

	if (I == 1)
	{
		int32_t Imm = ZERO_EXT_32(bits_get(operand2, 0, 7));
		int8_t rotate_amount = bits_get(operand2, 8, 11) << 1;
		Imm = rotate(Imm, rotate_amount);
		operand2 = Imm;
	}
	else
	{
		operand2 = immediate_shifted_register(operand2, S);

	} // end if(I == 1)

	int32_t result;

	int32_t operand1 = ARM->registers[Rn];
	switch (opCode)
	{
		case 0  :
		case 8  : // and, tst
		{
			result = operand1 & operand2;
			break;
		}
		case 1  :
		case 9  : // eor, teq
		{
			result = operand1 ^ operand2;
			break;
		}
		case 2  :
		case 10 : // sub, cmp
		{
			result = operand1 - operand2;
			break;
		}
		case 3  : // rsb
		{
			result = operand2 - operand1;
			break;
		}
		case 4  : // add
		{
			result = operand1 + operand2;
			break;
		}
		case 12 : // orr
		{
			result = operand1 | operand2;
			break;
		}
		case 13 : // mov
		{
			result = operand2;
			break;
		}
		default : result = 0;

	} // end switch (opCode)

	switch (opCode)
	{
		case 8  :
		case 9  :
		case 10 : // tst, teq, cmp discarded result
		{
			result = 0;
			break;
		}
		case 0  :
		case 1  :
		case 2  :
		case 3  :
		case 4  :
		case 12 :
		case 13 : // keep result
		{
			ARM->registers[Rd] = result;
			break;
		}
	}

	if (S == 1) // sets flags
	{
		if(result == 0) set_CPSR_flag(ZERO); 		// Z
		put_CPSR_flag(NEGATIVE, BIT_GET(result, 31));	// N

		switch (opCode)
		{
			case 4 : // add
			{
				if (get_CPSR_flag(OVERFLOW) == 1)
				{
					set_CPSR_flag(CARRY);
				}
				else clear_CPSR_flag(CARRY);
			}
			case 2  :
			case 3  :
			case 10 : // sub, rsb, cmp
			{
				if (get_CPSR_flag(OVERFLOW) == 0)
				{
					set_CPSR_flag(CARRY);
				}
				else clear_CPSR_flag(CARRY);
			}
		}
	}

}



// helper function that performs the shifting
// when shifted_register/offset is a shift register
int32_t immediate_shifted_register(int32_t word12, int8_t S)
{
	SHIFT_REGISTER *shift_register
			= (SHIFT_REGISTER *) &word12;

	int32_t shift_flag = shift_register->Shift_flag;
	int32_t shift_amount = 0;

	if (shift_flag == 0)
	{
		shift_amount = shift_register->Shift_amount;
	}
	else
	{
		int32_t register_number = shift_register->Shift_amount;
		BIT_CLEAR(shift_amount, 27);

		int32_t Rs   = ARM->registers[register_number];
		shift_amount = bits_get(Rs, 0, 8);
	}

	int32_t shift_type   = shift_register->Shift_type;
	int32_t Rm           = shift_register->Rm;

	int32_t shift_reg    = ARM->registers[Rm];

	switch (shift_type)
	{
		case 0 : //logical shit left
		{
			word12  = shift_reg << shift_amount;
			int carry = 0;
			if (shift_amount!=0)
			{
				carry = BIT_GET(shift_reg, 31-shift_amount+1);
			}
			if (S == 1) put_CPSR_flag(CARRY, carry);
			break;
		}
		case 1 : //logical right shift
		{
			word12  = shift_reg >> shift_amount;
			int carry = 0;
			if (shift_amount != 0)
			{
				carry = BIT_GET(shift_reg, shift_amount - 1);
			}
			if (S == 1) put_CPSR_flag(CARRY, carry);
			break;
		}
		case 2 : //arithmetic right shift
		{
			word12 = shift_reg >> shift_amount;
			int carry = 0;
			if (shift_amount != 0)
			{
				carry = BIT_GET(shift_reg, shift_amount - 1);
			}

			if (S == 1) put_CPSR_flag(CARRY, carry);

			int bit = BIT_GET(shift_reg, 31);
			for (int j=0; j<shift_amount; ++j)
			{
				bit_put(word12, 31 - j, bit);
			}
			break;
		}
		case 3 : //rotate right
		{
			word12 = rotate(shift_reg, shift_amount);
			break;
		}
		default : system_exit("INVALID INSTRUCTION FORMAT");
	} // end switch(shift_type)

	return word12;

}

void exe_multiply(int32_t word)
{

	assert(!BIT_GET(word, 25) && !BIT_GET(word, 26)
		&& BIT_GET(word, 4) && BIT_GET(word, 7));
 	assert(check_condition_code(word));

	MULTIPLY_INSTRUCTION *instruction
		= (MULTIPLY_INSTRUCTION *) &word;

	int32_t Rd = instruction->Rd;
	int32_t Rn = instruction->Rn;
	int32_t Rs = instruction->Rs;
	int32_t Rm = instruction->Rm;
	int A      = instruction->A;
	int S      = instruction->S;

	int32_t result = ARM->registers[Rm] * ARM->registers[Rs];

	if (A == 0) ARM->registers[Rd] = result;
	else
	{
		result += ARM->registers[Rn];
		ARM->registers[Rd] = result;
	}

	if (S == 1)
	{
		// N is set to bit 31 of the result and Z is set if and only if the result is zero.
		int result_bit = BIT_GET(result, 31);
		if (result_bit == 1)
			set_CPSR_flag(NEGATIVE);
		else
			clear_CPSR_flag(NEGATIVE);

		if (result == 0) set_CPSR_flag(ZERO);
	}
}


void exe_branch(int32_t word)
{
	assert(bits_get(word, 26, 27) == 1);
 	assert(check_condition_code(word));

	BRANCH_INSTRUCTION *instruction
		= (BRANCH_INSTRUCTION *) &word;

	int32_t offset = instruction->Offset;
	offset <<= 2;
	offset = ZERO_EXT_32(offset);

	ARM->registers[PC]    += (offset - 8);
	ARM->pipeline->fetched = ARM->memory[0];
	ARM->pipeline->decode  = ARM->memory[0];
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
	int32_t first  = ARM->memory[m+3]& EIGHT_BITS,
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

int32_t register_read(registerT reg)
{
	return 0;
}

void register_write(registerT reg, int32_t word)
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








