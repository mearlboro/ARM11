#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "libs/bitwise.h"

////  PLEASE FORGIVE ME <3  ////////////////////////////////////////////////////

#define forever while("You Are Not Upset")

//// D'AWWWWWWWWWWW ///////////////////// HUG //////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//// 1. ARM OBJECT  ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "libs/ARM.h"


////////////////////////////////////////////////////////////////////////////////
//// 2. INSTRUCTION DEFINITONS  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "libs/instruction.h"


////////////////////////////////////////////////////////////////////////////////
//// 3. MACROS/PROTOTYPES  /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////  3.1 REGISTERS READ/WRITE  ////////////////////////////////////////////////

#define REG_READ(r)     (ARM->registers[(r)])
#define REG_WRITE(r, v) (ARM->registers[(r)] = (v))


////  3.2 MEMEORY READ/WRITE  //////////////////////////////////////////////////

#define MEM_BYTE_READ(i)      (ARM->memory[(i)])

#define MEM_WORD_READ(i)     ((MEM_BYTE_READ(i+3) & 0xFF) << 24 | \
                              (MEM_BYTE_READ(i+2) & 0xFF) << 16 | \
                              (MEM_BYTE_READ(i+1) & 0xFF) <<  8 | \
                              (MEM_BYTE_READ(i+0) & 0xFF) <<  0 )

#define MEM_WORD_READ_BE(i)  ((MEM_BYTE_READ(i+0) & 0xFF) << 24 | \
                              (MEM_BYTE_READ(i+1) & 0xFF) << 16 | \
                              (MEM_BYTE_READ(i+2) & 0xFF) <<  8 | \
                              (MEM_BYTE_READ(i+3) & 0xFF) <<  0 )

#define MEM_BYTE_WRITE(i, b)  (ARM->memory[(i)] = (b))                      

#define MEM_WORD_WRITE(i, w) { MEM_BYTE_WRITE(i+0, ((w) >>  0) & 0xFF); \
                               MEM_BYTE_WRITE(i+1, ((w) >>  8) & 0xFF); \
                               MEM_BYTE_WRITE(i+2, ((w) >> 16) & 0xFF); \
                               MEM_BYTE_WRITE(i+3, ((w) >> 24) & 0xFF); }


////  3.3 CPSR READ/WRITE  /////////////////////////////////////////////////////

#define CPSR_CLR(f)    (BIT_CLR(REG_READ(CPSR), (f)))
#define CPSR_SET(f)    (BIT_SET(REG_READ(CPSR), (f)))
#define CPSR_GET(f)    (BIT_GET(REG_READ(CPSR), (f)))
#define CPSR_PUT(f, b) { if (IS_SET(b)) CPSR_SET(f); else CPSR_CLR(f); }


////  3.4 PC INCREMENT  ////////////////////////////////////////////////////////

#define INCREMENT_PC(a) (ARM->registers[PC] += (a))                         

 
////  3.5 FUNCTION PROTOTYPES  /////////////////////////////////////////////////

void read_ARM_program(const char *);
void print_ARM_state(void);

void emulate(void);
int check_condition_code(int32_t);
void decode_instruction(int32_t);
void exe_single_data_transfer(int32_t);
void exe_data_processing(int32_t);
void exe_multiply(int32_t);
void exe_branch(int32_t);

int32_t as_shifted_reg(int32_t, int8_t);
int32_t as_immediate_reg(int);

int is_GPIO_address(int);
void print_GPIO_address(int);

////////////////////////////////////////////////////////////////////////////////
////  4. MAIN  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////  4.0 ARM OBJECT  //////////////////////////////////////////////////////////

ARMState *ARM = NULL;


////  4.1 MAIN  ////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	// Make sure input file is provided
	if (argc < 2) 
	{ 
		printf("\"emulate <input_file>\""); 
		exit(EXIT_FAILURE); 
	}
	
	// Allocate memory for the ARM
	ARM = calloc(1, sizeof(ARMState));
	ARM->pipeline = calloc(1, sizeof(Pipeline));
	if (ARM == NULL) 
	{
		printf("MEMORY ERROR\n"); 
		exit(EXIT_FAILURE); 
	}
	
	// Read input file and emulate
	read_ARM_program(argv[1]);
	emulate();
	
	// Free memory and exit program
	free(ARM);
	free(ARM->pipeline);

	return EXIT_SUCCESS;
}


////  4.2 READ ARM PROGRAM  ///////////////////////////////////////////////////

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


////  4.3 PRINT ARM STATE  ////////////////////////////////////////////////////

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



////////////////////////////////////////////////////////////////////////////////
////  5. CORE  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

 
////  5.1 EMULATE  /////////////////////////////////////////////////////////////

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

////  5.2 INSTRUCTION DECODER  /////////////////////////////////////////////////

void decode_instruction(int32_t word)
{
	int code = bits_get(word, 26, 27);
	switch (code)
	{
		case 1 : exe_single_data_transfer(word); break;
		case 2 : exe_branch(word);               break;
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

////  5.3 CHECK CONDITION CODE  ///////////////////////////////////////////////

int check_condition_code(int32_t word)
{
	int cond = bits_get(word, 28, 31);

	switch (cond)
	{
		case EQ : return  CPSR_GET(ZERO);
		case NE : return !CPSR_GET(ZERO);
		case GE : return  CPSR_GET(NEGATIVE) == CPSR_GET(OVERFLOW);
		case LT : return  CPSR_GET(NEGATIVE) != CPSR_GET(OVERFLOW);
		case GT : return !CPSR_GET(ZERO) && 
										 (CPSR_GET(NEGATIVE) == CPSR_GET(OVERFLOW));
		case LE : return  CPSR_GET(ZERO) || 
										 (CPSR_GET(NEGATIVE) != CPSR_GET(OVERFLOW));
		case AL : return 1;
		default : return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
////  5.4 IMPLEMENT INSTRUCTIONS  /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////  5.4.1 SINGLE DATA TRANSFER  /////////////////////////////////////////////

void exe_single_data_transfer(int32_t word)
{
	SingleDataTransferInstr *instr = (SingleDataTransferInstr *) &word;
	
	int Rn     = instr->Rn;         // base register
	int Rd     = instr->Rd;         // destination register
	int Offset = instr->Offset;
	int I      = instr->_I;
	int P      = instr->P;
	int U      = instr->U;
	int L      = instr->L;

	int PostIndexing = IS_CLEAR(P);
	int PreIndexing  = !PostIndexing;
	
	int address = ARM->registers[Rn];
	int value   = ARM->registers[Rd];
	
	Offset = IS_CLEAR(I) ? as_immediate_reg(Offset) 
											 : as_shifted_reg(Offset, 0);
	
	if (PreIndexing) address += (IS_SET(U) ? Offset : -Offset);

	if (address < 0 || address >= MEMORY_CAPACITY) goto moob;
	if (is_GPIO_address(address))                  goto gpio;

	if (IS_SET(L)) REG_WRITE(Rd, MEM_WORD_READ(address));
	else           MEM_WORD_WRITE(address, value);
	
	if (PostIndexing) REG_WRITE(Rn, address += (IS_SET(U) ? Offset : -Offset));

	if (address < 0 || address >= MEMORY_CAPACITY) goto moob;
	if (is_GPIO_address(address))                  goto gpio;


	return;

	moob:
			if(!is_GPIO_address(address)) 
			{		
				printf("Error: Out of bounds memory access at address 0x%08x\n", address);
				return; 
			}
	
	gpio: 
			print_GPIO_address(address);
}


////  5.4.2 DATA PROCESSING  ///////////////////////////////////////////////////

void exe_data_processing(int32_t word)
{
	DataProcessingInstr *inst = (DataProcessingInstr *) &word;
	
	int I        = inst->_I;        // 25
	int OpCode   = inst->OpCode;   // 24-21
	int S        = inst->S;        
	int Rn       = inst->Rn;
	int Rd       = inst->Rd;
	int Operand2 = inst->Operand2; // 11-0
	
	int Operand1 = ARM->registers[Rn];

	Operand2     = IS_CLEAR(I) ? as_shifted_reg(Operand2, S) 
	           		           : as_immediate_reg(Operand2);
	int result   = 0;

	// calculate result by opcode
	switch (OpCode)
	{
		case AND :
		case TST : result = Operand1 & Operand2; break;
		case EOR :
		case TEQ : result = Operand1 ^ Operand2; break; 
		case SUB :
		case CMP : result = Operand1 - Operand2; break; 
		case RSB : result = Operand2 - Operand1; break; 
		case ADD : result = Operand1 + Operand2; break; 
		case ORR : result = Operand1 | Operand2; break; 
		case MOV : result = Operand2;            break; 
		default  : result = 0;
	}
	
	// save results if necessary
	switch (OpCode)
	{
		case TST :
		case TEQ :
		case CMP :                        break;
		default  : REG_WRITE(Rd, result); break; 
	}
	
	if (IS_CLEAR(S)) return;
	
	// set flags
	CPSR_PUT(ZERO, (result == 0));    
	CPSR_PUT(NEGATIVE, BIT_GET(result, 31));	
	switch (OpCode)
	{
		case 2  :
		case 3  :
		case 10 : CPSR_PUT(CARRY, (result >= 0));      break; 
		case 4  : CPSR_PUT(CARRY, CPSR_GET(OVERFLOW)); break; 
	}
}


////  5.4.3 MULTIPLY INSTRUCTION  //////////////////////////////////////////////

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
	CPSR_PUT(NEGATIVE, BIT_GET(result, 31)); // N is bit 31 of the result
	CPSR_PUT(ZERO, (result == 0));           // Z is set iff result is 0
}


////  5.4.4 BRANCH INSTRUCTION  ////////////////////////////////////////////////

void exe_branch(int32_t word)
{
	BranchInstr *instr = (BranchInstr *) &word;
	
	// Sign extend 32-bit and shift left 2
	int32_t offset = (((instr->Offset) << 2) << 6) >> 6;
	
	INCREMENT_PC(offset);
	
	ARM->pipeline->fetched = MEM_WORD_READ(REG_READ(PC));
	INCREMENT_PC(4);
}


////  5.4.5 SHIFTING ///////////////////////////////////////////////////////////

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
	
	int Flag     = sreg->Flag;
	int Type     = sreg->Type;
	int Rm       = sreg->Rm;
	int amount   = sreg->Amount;
	uint32_t reg = REG_READ(Rm);
	int carry    = 0;
	
	if (IS_SET(Flag))
	{
		int Rs = REG_READ(bits_get(amount, 1, 4));
		amount = bits_get(Rs, 0, 8);
	}
	
	switch (Type)
	{
		case 0 : // lsl - Logical shift left
		{
			value = reg << amount;
			if (amount != 0) carry = BIT_GET(reg, 31 - amount + 1);
			if (IS_SET(S))   CPSR_PUT(CARRY, carry);
			break;
		}
		case 1 : // lsr - Logical shift right
		{
			value = reg >> amount;
			if (amount != 0) carry = BIT_GET(reg, amount - 1);
			if (IS_SET(S))   CPSR_PUT(CARRY, carry);
			break;
		}
		case 2 : // asr - Arithmetic shift right
		{
			value = reg >> amount;
			if (amount != 0) carry = BIT_GET(reg, amount - 1);
			if (S == 1)      CPSR_PUT(CARRY, carry);
			int bit = BIT_GET(reg, 31); // TODO move to bits set
			for (int j = 0; j < amount; j++) BIT_PUT(value, 31 - j, bit);
			break;
		}
		case 3  : // ror - Rotate right
		{
			value = rotate_right(reg, amount); 
			break; 
		}
		default : exit(EXIT_FAILURE);
	}
	
	return value;
}


///////////////////////////////////////////////////////////////////////////////
////  5.5 GPIO ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int pins[32];

int is_GPIO_address(int address) {

	switch(address) 
	{
		case 0x20200000:
		case 0x20200004:
		case 0x20200008: 
		case 0x20200028:
		case 0x2020001c: return 1;
		default:         return 0;
	}
} 

void print_GPIO_address(int address) 
{
	int group_of_pins = 0;
	int pin_on        = 0;
	int pin_off       = 0; 
	
	switch(address) 
	{
		case 0x20200000: group_of_pins =  0; goto pin_accessed;
		case 0x20200004: group_of_pins = 10; goto pin_accessed;
		case 0x20200008: group_of_pins = 20; goto pin_accessed;
		case 0x20200028: pin_off       =  1; break;
		case 0x2020001c: pin_on        =  1; break;
		default:         return;
	}

	if (pin_on)  printf("PIN ON\n");
	if (pin_off) printf("PIN OFF\n");

	return; 

	pin_accessed:
		printf("One GPIO pin from %d to %d has been accessed\n", 
						group_of_pins, group_of_pins + 9);
}

