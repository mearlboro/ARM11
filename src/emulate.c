#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "bitwise.h"

////  PLEASE FORGIVE ME <3  ////////////////////////////////////////////////////

#define forever while("You Are Not Upset")

////  CONSTANTS  ///////////////////////////////////////////////////////////////

#define MEMORY_CAPACITY 65536
#define REGISTER_COUNT  17

////  REGISTERS READ/WRITE  ////////////////////////////////////////////////////

#define REG_READ(r)     (ARM->registers[(r)])
#define REG_WRITE(r, v) (ARM->registers[(r)] = (v))

////  MEMEORY READ/WRITE  //////////////////////////////////////////////////////

#define MEM_BYTE_READ(i)      (ARM->memory[(i)])

#define MEM_WORD_READ(i)     ((MEM_BYTE_READ(i+3) & 0xFF) << 24 | \
                              (MEM_BYTE_READ(i+2) & 0xFF) << 16 | \
                              (MEM_BYTE_READ(i+1) & 0xFF) <<  8 | \
                              (MEM_BYTE_READ(i+0) & 0xFF) <<  0 )

#define MEM_WORD_READ_BE(i)  ((MEM_BYTE_READ(i+0) & 0xFF) << 24 | \
                              (MEM_BYTE_READ(i+1) & 0xFF) << 16 | \
                              (MEM_BYTE_READ(i+2) & 0xFF) <<  8 | \
                              (MEM_BYTE_READ(i+3) & 0xFF) <<  0 )

#define MEM_BYTE_WRITE(i, b)  (ARM->memory[i] = (b))                      /**/

#define MEM_WORD_WRITE(i, w) { MEM_BYTE_WRITE(i+0, ((w) >>  0) & 0xFF); \
                               MEM_BYTE_WRITE(i+1, ((w) >>  8) & 0xFF); \
                               MEM_BYTE_WRITE(i+2, ((w) >> 16) & 0xFF); \
                               MEM_BYTE_WRITE(i+3, ((w) >> 24) & 0xFF); }

////  CPSR/PC READ/WRITE  //////////////////////////////////////////////////////

#define CPSR_CLR(f)    (BIT_CLR(REG_READ(CPSR), (f)))
#define CPSR_SET(f)    (BIT_SET(REG_READ(CPSR), (f)))
#define CPSR_GET(f)    (BIT_GET(REG_READ(CPSR), (f)))
#define CPSR_PUT(f, b) { if (IS_SET(b)) CPSR_SET(f); else CPSR_CLR(f); }

#define INCREMENT_PC(a) (ARM->registers[PC] += (a))                         /**/

////  INSTRUCTION TYPE DEFINITIONS  ////////////////////////////////////////////

typedef struct DataProcessingInstr
{
	unsigned int Operand2 : 12;
	unsigned int Rd       : 4;
	unsigned int Rn       : 4;
	unsigned int S        : 1;
	unsigned int OpCode   : 4;
	unsigned int I        : 1;
	unsigned int _00      : 2;
	unsigned int Cond     : 4;
} DataProcessingInstr;


typedef struct ImmediateReg
{
	unsigned int Imm    : 8;
	unsigned int Rotate : 4;
} ImmediateReg;


typedef struct ShiftReg
{
	unsigned int Rm     : 4;
	unsigned int Flag   : 1;
	unsigned int Type   : 2;
	unsigned int Amount : 5;
} ShiftReg;


typedef struct MultiplyInstr
{
	unsigned int Rm      : 4;
	unsigned int _1001   : 4;
	unsigned int Rs      : 4;
	unsigned int Rn      : 4;
	unsigned int Rd      : 4;
	unsigned int S       : 1;
	unsigned int A       : 1;
	unsigned int _000000 : 6;
	unsigned int Cond    : 4;
} MultiplyInstr;


typedef struct SingleDataTransferInstr
{
	unsigned int Offset : 12;
	unsigned int Rd     : 4;
	unsigned int Rn     : 4;
	unsigned int L      : 1;
	unsigned int _00    : 2;
	unsigned int U      : 1;
	unsigned int P      : 1;
	unsigned int I      : 1;
	unsigned int _01    : 2;
	unsigned int Cond   : 4;
} SingleDataTransferInstr;


typedef struct BranchInstr
{
	unsigned int Offset : 24;
	unsigned int _1010  : 4;
	unsigned int Cond   : 4;
} BranchInstr;


typedef struct Pipeline
{
	int32_t fetched;
	int32_t decoded;
} Pipeline;


typedef struct ARMState
{
	int8_t memory[MEMORY_CAPACITY];
	int32_t registers[REGISTER_COUNT];
	Pipeline *pipeline;
} ARMState;


typedef enum
{
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12,
	SP, LR,
	PC,
	CPSR
} Register;


typedef enum
{
	NEGATIVE = 31,
	ZERO 	 = 30,
	CARRY 	 = 29,
	OVERFLOW = 28
} CPSRFlag;

////  ARM OBJECT  //////////////////////////////////////////////////////////////

ARMState *ARM = NULL;

////  FUNCTION PROTOTYPES  /////////////////////////////////////////////////////

void read_ARM_program(const char *filename);
void print_ARM_state();

void emulate();
int check_condition_code(int32_t word);
void decode_instruction(int32_t word);
void exe_single_data_transfer(int32_t word);
void exe_data_processing(int32_t word);
void exe_multiply(int32_t word);
void exe_branch(int32_t word);

int32_t as_shifted_reg(int32_t value, int8_t S);
int32_t as_immediate_reg(int value);

////  MAIN  ////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	// Make sure input file is provided
	if (argc < 2) { printf("\"emulate <input_file>\""); exit(EXIT_FAILURE); }
	
	// Allocate memory for the ARM
	ARM = calloc(1, sizeof(ARMState));
	ARM->pipeline = calloc(1, sizeof(Pipeline));
	if (ARM == NULL) { printf("MEMORY ERROR\n"); exit(EXIT_FAILURE); }
	
	// Read input file and emulate
	read_ARM_program(argv[1]);
	//"/Users/Zeme/ARM11_XCODE/ARM11_XCODE/test_cases/add01");//argv[1]);
	emulate();
	
	// Free memory and exit program
	free(ARM);
	free(ARM->pipeline);
	return EXIT_SUCCESS;
}

//////////  READ ARM PROGRAM  //////////////////////////////////////////////////

void read_ARM_program(const char *filename)
{
	FILE *file;

	if ((file = fopen(filename, "rb")) == NULL)      goto error; // Open file
	if (fseek(file, 0, SEEK_END) != 0)               goto error; // Go to end
	long bytes = ftell(file);                                    // Read bytes
	rewind(file);                                                // Back to start
	if (fread(ARM->memory, 1, bytes, file) != bytes) goto error; // Load in ARM
	fclose(file);                                                // Close file
	if (ferror(file))                                goto error; // Error check
	return;

error:
	perror("FILE ERROR");
	exit(EXIT_FAILURE);
}

//////////  PRINT ARM STATE  ///////////////////////////////////////////////////

void print_ARM_state()
{
	// Print contents of registers
	printf("Registers:\n");
	for (int i = 0; i < REGISTER_COUNT - 4; i++)
	{
		printf("$%-3i: %10i (0x%08x)\n", i, REG_READ(i), REG_READ(i));
	}
	printf("PC  : %10i (0x%08x)\n", REG_READ(PC),   REG_READ(PC));
	printf("CPSR: %10i (0x%08x)\n", REG_READ(CPSR), REG_READ(CPSR));
	
	// Print non-zero memory
	printf("Non-zero memory:\n");
	for (int i = 0; i < MEMORY_CAPACITY; i += 4)
	{
		if (MEM_WORD_READ(i) == 0) continue;
		printf("0x%08x: 0x%08x\n", i, MEM_WORD_READ_BE(i));
	}
}

//////////  EMULATE ////////////////////////////////////////////////////////////

void emulate()
{
	REG_WRITE(PC, 0);
	ARM->pipeline->fetched = MEM_WORD_READ(REG_READ(PC));
	INCREMENT_PC(4);
	
	forever
	{
		ARM->pipeline->decoded = ARM->pipeline->fetched;
		ARM->pipeline->fetched = MEM_WORD_READ(REG_READ(PC));
		INCREMENT_PC(4);
		
		int32_t instr = ARM->pipeline->decoded;
		if (instr == 0) { print_ARM_state(); return; }
		
		if (check_condition_code(instr))
		{
			decode_instruction(ARM->pipeline->decoded);
		}
	}
}

//////////  DECODE INSTRUCTION  ////////////////////////////////////////////////

void decode_instruction(int32_t word)
{
	int code = bits_get(word, 26, 27);
	switch (code)
	{
		case 1 :                            exe_single_data_transfer(word); break;
		case 2 :                            exe_branch(word);               break;
		case 0 :
		{
			if (IS_SET(BIT_GET(word, 25)))    exe_data_processing(word);
			else
			{
				if (IS_CLEAR(BIT_GET(word, 4))) exe_data_processing(word);
				else
				{
					if (IS_SET(BIT_GET(word, 7))) exe_multiply(word);
					else                          exe_data_processing(word);
				}
			}
			break;
		}
		default : exit(EXIT_FAILURE);
	}
}

//////////  CHECK CONDITION CODE  //////////////////////////////////////////////

int check_condition_code(int32_t word)
{
	int cond = bits_get(word, 28, 31);
	switch (cond)
	{
		case 0  : return  CPSR_GET(ZERO);
		case 1  : return !CPSR_GET(ZERO);
		case 10 : return  CPSR_GET(NEGATIVE) == CPSR_GET(OVERFLOW);
		case 11 : return  CPSR_GET(NEGATIVE) != CPSR_GET(OVERFLOW);
		case 12 : return !CPSR_GET(ZERO)&& (CPSR_GET(NEGATIVE)==CPSR_GET(OVERFLOW));
		case 13 : return  CPSR_GET(ZERO)|| (CPSR_GET(NEGATIVE)!=CPSR_GET(OVERFLOW));
		case 14 : return 1;
		default : return 0;
	}
}

//////////  SINGLE DATA TRANSFER ///////////////////////////////////////////////

void exe_single_data_transfer(int32_t word)
{
	SingleDataTransferInstr *instr = (SingleDataTransferInstr *) &word;
	
	int Rn     = instr->Rn;         // base register
	int Rd     = instr->Rd;         // destination register
	int Offset = instr->Offset;
	int I      = instr->I;
	int P      = instr->P;
	int U      = instr->U;
	int L      = instr->L;
	
	int PostIndexing = IS_CLEAR(P);
	int PreIndexing  = !PostIndexing;
	
	int address = ARM->registers[Rn];
	int value   = ARM->registers[Rd];
	
	Offset = IS_CLEAR(I) ? as_immediate_reg(Offset) : as_shifted_reg(Offset, 0);
	
	if (PreIndexing) address += IS_SET(U) ? Offset : -Offset;
	
	if (address >= MEMORY_CAPACITY || address < 0) goto moob;

	if (IS_SET(L)) REG_WRITE(Rd, MEM_WORD_READ(address));
	else           MEM_WORD_WRITE(address, value);
	
	if (PostIndexing) REG_WRITE(Rn, address += (IS_SET(U) ? Offset : -Offset));
	
	if (address >= MEMORY_CAPACITY || address < 0) goto moob;
	
	return;

moob:
	printf("Error: Out of bounds memory access at address 0x%08x\n", address);
	return;
}

//////////  DATA PROCESSING  ///////////////////////////////////////////////////

void exe_data_processing(int32_t word)
{
	DataProcessingInstr *inst = (DataProcessingInstr *) &word;
	
	int I        = inst->I;             // 25
	int OpCode   = inst->OpCode;        // 24-21
	int S        = inst->S;
	int Rn       = inst->Rn;
	int Rd       = inst->Rd;
	int Operand2 = inst->Operand2;      // 11-0
	
	int Operand1 = ARM->registers[Rn];
	int result   = 0;
	
	Operand2 = IS_CLEAR(I) ? as_shifted_reg(Operand2, S) 
	                       : as_immediate_reg(Operand2);
	
	switch (OpCode)
	{
		case 0  :
		case 8  : result = Operand1 & Operand2; break; // and, tst
		case 1  :
		case 9  : result = Operand1 ^ Operand2; break; // eor, teq
		case 2  :
		case 10 : result = Operand1 - Operand2; break; // sub, cmp
		case 3  : result = Operand2 - Operand1; break; // rsb
		case 4  : result = Operand1 + Operand2; break; // add
		case 12 : result = Operand1 | Operand2; break; // orr
		case 13 : result = Operand2;            break; // mov
		default : result = 0;
	}
	
	switch (OpCode)
	{
		case 8  :
		case 9  :
		case 10 :                        break; // tst, teq, cmp result not written
		default : REG_WRITE(Rd, result); break; // keep result
	}
	
	if (IS_CLEAR(S)) return;
		
	CPSR_PUT(ZERO, (result == 0));    
	CPSR_PUT(NEGATIVE, BIT_GET(result, 31));	
	switch (OpCode)
	{
		case 2  :
		case 3  :
		case 10 : CPSR_PUT(CARRY, (result >= 0));      break; // sub, rsb, cmp
		case 4  : CPSR_PUT(CARRY, CPSR_GET(OVERFLOW)); break; // add
	}
}

//////////  MULTIPLY INSTRUCTION  //////////////////////////////////////////////

void exe_multiply(int32_t word)
{
	MultiplyInstr *instr = (MultiplyInstr *) &word;
	
	int Rd = instr->Rd;
	int Rn = instr->Rn;
	int Rs = instr->Rs;
	int Rm = instr->Rm;
	int A  = instr->A;
	int S  = instr->S;
	
	int result = REG_READ(Rm) * REG_READ(Rs);
	
	if (IS_CLEAR(A)) REG_WRITE(Rd, result);
	else             REG_WRITE(Rd, result += REG_READ(Rn));
	
	if (IS_CLEAR(S)) return;
	CPSR_PUT(NEGATIVE, BIT_GET(result, 31)); // N is set to bit 31 of the result
	CPSR_PUT(ZERO, (result == 0));           // Z is set iff the result is zero
}

//////////  BRANCH INSTRUCTION  //////////////////////////////////////////////

void exe_branch(int32_t word)
{
	BranchInstr *instr = (BranchInstr *) &word;
	
	// Sign extend 32-bit and shift left 2
	int32_t offset = (((instr->Offset) << 2) << 6) >> 6;
	
	INCREMENT_PC(offset);
	
	// TODO don't like this here
	ARM->pipeline->fetched = MEM_WORD_READ(REG_READ(PC));
	INCREMENT_PC(4);
}

////  AS IMMEDIATE REGISTER  ///////////////////////////////////////////////////

int32_t as_immediate_reg(int value)
{
	int Imm = bits_get(value, 0, 7);
	int Rotate = bits_get(value, 8, 11) * 2;
	return rotate_right(Imm, Rotate);
}

////  AS SHIFT REGISTER  ///////////////////////////////////////////////////////

int32_t as_shifted_reg(int32_t value, int8_t S)
{
	ShiftReg *sreg = (ShiftReg *) &value;
	
	int Flag   = sreg->Flag;
	int Type   = sreg->Type;
	int Rm     = sreg->Rm;
	int amount = sreg->Amount;
	int reg    = REG_READ(Rm);
	int carry  = 0;
	
	if (IS_SET(Flag))
	{
		int Rs = REG_READ(/*rnum=sreg->Amount*/amount);
		amount = bits_get(Rs, 0, 8);
	}
	
	switch (Type)
	{
		case 0 : // Logical shit left
		{
			value = reg << amount;
			if (amount != 0) carry = BIT_GET(reg, 31 - amount + 1);
			if (IS_SET(S))   CPSR_PUT(CARRY, carry);
			break;
		}
		case 1 : // Logical right shift
		{
			value = reg >> amount;
			if (amount != 0) carry = BIT_GET(reg, amount - 1);
			if (IS_SET(S))   CPSR_PUT(CARRY, carry);
			break;
		}
		case 2 : // Arithmetic right shift
		{
			value = reg >> amount;
			if (amount != 0) carry = BIT_GET(reg, amount - 1);
			if (S == 1)      CPSR_PUT(CARRY, carry);
			int bit = BIT_GET(reg, 31); // TODO move to bits set
			for (int j = 0; j < amount; j++) BIT_PUT(value, 31 - j, bit);
			break;
		}
		case 3  : value = rotate_right(reg, amount); break;  //rotate right
		default : exit(EXIT_FAILURE);
	}
	
	return value;
}

