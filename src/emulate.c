#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "bitwise.h"

//////////  CONSTANTS AND MACROS  //////////////////////////////////////////////

#define MEMORY_CAPACITY 65536
#define REGISTER_COUNT 17

//////////  INSTRUCTION TYPE DEFINITIONS ///////////////////////////////////////

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


typedef struct
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
	ZERO 	   = 30,
	CARRY 	 = 29,
	OVERFLOW = 28
} CPSRFlag;

//////////  ARM OBJECT  ////////////////////////////////////////////////////////

ARMState *ARM = NULL;

//////////  FUNCTION PROTOTYPES  ///////////////////////////////////////////////

void emulator_loop();
void decode_instruction(int32_t word);
void exe_single_data_transfer(int32_t word);
void exe_data_processing(int32_t word);
void exe_multiply(int32_t word);
void exe_branch(int32_t word);
int32_t immediate_shifted_register(int32_t word12, int8_t S);
int8_t memory_byte_read(uint16_t memory_address);
int32_t memory_word_read(uint16_t memory_address);
void memory_byte_write(uint16_t memory_address, int8_t byte);
void memory_word_write(uint16_t memory_address, int32_t word);
int32_t register_read(Register reg);
void register_write(Register reg, int32_t word);
int check_condition_code(int32_t word);
int get_CPSR_flag(CPSRFlag flag);

void print_ARM_info();
void system_exit(char *message);
int32_t test_glue(int8_t a, int8_t b, int8_t c, int8_t d);

void print_ARM_state();
void read_ARM_program(const char *filename);

//////////  BINARY FILE READ  //////////////////////////////////////////////////

void read_ARM_program(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL)
  {
		system_exit("Error opening input file");
  }
  
	if (fseek(file, 0, SEEK_END) != 0)
  {
		system_exit("Error reading end of file");
  }
	long bytes = ftell(file);
	rewind(file);
  
	if (fread(ARM->memory, 1, bytes, file) != bytes)
  {
		system_exit("Error parsing file into ARM memory");
  }
  
	if (ferror(file))
  {
		system_exit("Error working with file");
  }
	fclose(file);
}

//////////  PRINT ARM STATE  ///////////////////////////////////////////////////

void print_ARM_state()
{
	printf("Registers:\n");
	for (int i = 0; i < REGISTER_COUNT - 4; i++)
	{
		printf("$%-3i:%11i (0x%08x)\n", i, ARM->registers[i], ARM->registers[i]);
	}
  
	printf("PC  : %10i (0x%08x)\n", ARM->registers[PC], ARM->registers[PC]);
	printf("CPSR: %10i (0x%08x)\n", ARM->registers[CPSR], ARM->registers[CPSR]);
  
	printf("Non-zero memory:\n");
  for (int i = 0; i < MEMORY_CAPACITY; i += 4)
	{
		if (memory_word_read(i) == 0) continue;
    
		printf("0x%08x: 0x%02x%02x%02x%02x\n", i,
           ARM->memory[i]     & 0xFF,
           ARM->memory[i + 1] & 0xFF,
           ARM->memory[i + 2] & 0xFF,
           ARM->memory[i + 3] & 0xFF
           );
    
	}
}

//////////  MAIN  //////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	printf("Running ARM Emulator v1.0 (%s)\n", argv[0]);
	//if (argc < 2 || argv[1] == NULL)
  //{
  //	system_exit("Usage: emulate <input.bin>\nTerminated\n");
  //}
  
	// Allocating memory for the ARM
	ARM = calloc(1, sizeof(ARMState));
	ARM->pipeline = calloc(1, sizeof(Pipeline));
	if (ARM == NULL)
  {
    system_exit("Fatal memory-related error");
  }
  
	// Read binary file
	read_ARM_program("/Users/Zeme/ARM11_XCODE/ARM11_XCODE/test_cases/gpio_1");//argv[1]);
  
  // Begin execution
	emulator_loop();
  
  // Free stuff
	free(ARM);
  free(ARM->pipeline);
  
  // Everything went better than expected :)
	return EXIT_SUCCESS;
}

void system_exit(char *message)
{
	perror(message);
  exit(EXIT_FAILURE);
}

//////////  EMULATOR LOOP //////////////////////////////////////////////////////

void emulator_loop()
{
  ARM->registers[PC] = 0;
  
	ARM->pipeline->fetched = memory_word_read(ARM->registers[PC]);
	ARM->registers[PC] += 4;
  
	for (;;)
	{
		ARM->pipeline->decoded = ARM->pipeline->fetched;
    ARM->pipeline->fetched = memory_word_read(ARM->registers[PC]);
		ARM->registers[PC] += 4;
    
		int32_t exe_instruction = ARM->pipeline->decoded;
		if (exe_instruction == 0)
    {
      print_ARM_state();
      return;
    }
    printf("##############################################################\n");
    printf("%-20s", "Instruction: ");
    print_bits(exe_instruction);
    
		if (check_condition_code(exe_instruction))
		{
			decode_instruction(ARM->pipeline->decoded);
      print_ARM_state();
		}
	}
}

//////////  CPSR FLAG OPERATIONS  //////////////////////////////////////////////

#define CPSRclr(f)    (BIT_CLEAR(ARM->registers[CPSR], (f)))
#define CPSRset(f)      (BIT_SET(ARM->registers[CPSR], (f)))
#define CPSRget(f)      (BIT_GET(ARM->registers[CPSR], (f)))
#define CPSRput(f, b) if (IS_SET(b)) CPSRset(f); else CPSRclr(f);

void clear_CPSR_flag(CPSRFlag flag) { BIT_CLEAR(ARM->registers[CPSR], flag); }
void set_CPSR_flag(CPSRFlag flag) { BIT_SET(ARM->registers[CPSR], flag); }
int get_CPSR_flag(CPSRFlag flag) { return BIT_GET(ARM->registers[CPSR], flag); }
void put_CPSR_flag(CPSRFlag flag, int bit)
{
  if (bit == 1) set_CPSR_flag(flag);
  else clear_CPSR_flag(flag);
}

//////////  DECODE INSTRUCTION  ////////////////////////////////////////////////

void decode_instruction(int32_t word)
{
  //int code = bits_get(word, 26, 27);
  int code = bits_get2(word, 26, 27);//BITS_GET(word, 26, 27);
  printf("%-20s", "Decoding (CODE): ");
  print_bits(code);
  // DEBUG
  //print_bits(word);
  // END DEBUG
  
  switch (code)
  {
		case 1 : exe_single_data_transfer(word); break;
    case 2 : exe_branch(word); break;
    case 0 :
    {
      if (IS_SET(BIT_GET(word, 25))) exe_data_processing(word);
      else
      {
        if (IS_CLEAR(BIT_GET(word, 4))) exe_data_processing(word);
        else
        {
          if (IS_SET(BIT_GET(word, 7))) exe_multiply(word);
          else exe_data_processing(word);
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
  
	int cond = bits_get2(word, 28, 31); //BITS_GET(word, 28, 31);
  // DEBUG
  //print_bits(word);
  printf("%-20s", "Condition: ");
  print_bits(cond);
  // END DEBUG
	switch (cond)
	{
		case 0  : return  CPSRget(ZERO);
		case 1  : return !CPSRget(ZERO);
		case 10 : return  CPSRget(NEGATIVE) == CPSRget(OVERFLOW);
		case 11 : return  CPSRget(NEGATIVE) != CPSRget(OVERFLOW);
		case 12 : return !CPSRget(ZERO) && (CPSRget(NEGATIVE) == CPSRget(OVERFLOW));
    case 13 : return  CPSRget(ZERO) || (CPSRget(NEGATIVE) != CPSRget(OVERFLOW));
		case 14 : return 1;
		default : return 0;
	}
  /*switch (cond)
   {
   // eq - Z set - equal
   case 0  : return get_CPSR_flag(ZERO);
   
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
   }*/
}

//////////  SINGLE DATA TRANSFER ///////////////////////////////////////////////




void exe_single_data_transfer(int32_t word)
{
	SingleDataTransferInstr *instr = (SingleDataTransferInstr *) &word;
  
	int Rn     = instr->Rn; // base register
	int Rd     = instr->Rd; // destination register
	int Offset = instr->Offset;
	int I      = instr->I;
	int P      = instr->P;
	int U      = instr->U;
	int L      = instr->L;
  
  // TODO move this to separate function
	if (IS_CLEAR(I))
	{
		//int32_t Imm = ZERO_EXT_32(BITS_GET(Offset, 0, 7));
    int Imm = bits_get2(Offset, 0, 7);
		int ramount = bits_get2(Offset, 8, 11) * 2;
		Offset = rotate(Imm, ramount);
	}
	else Offset = immediate_shifted_register(Offset, 0);
  
  int PostIndexing = IS_CLEAR(P);
  int PreIndexing = !PostIndexing;
  
  int address = ARM->registers[Rn];
  int value = ARM->registers[Rd];
  if (PreIndexing)
  {
    // Pre-indexing add offset to base register
    address += IS_SET(U) ? Offset : -Offset;
  }
  
  // Loading from memory into Rd
	if (IS_SET(L))
  {
    // MEMORY LOAD
    ARM->registers[Rd] = memory_word_read(address); //
  }
	else
  {
    // MEMORY STORE
    memory_word_write(address, value);
  }
	
  if (PostIndexing) {
    address += IS_SET(U) ? Offset : -Offset;
    ARM->registers[Rn] = address;
  }

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
	int result;
  
  // 11100011 10100000 00111100 00000001
  switch (I)
  {
    case 0 : Operand2 = immediate_shifted_register(Operand2, S); break;
    case 1 :
    {
      //int32_t Imm = ZERO_EXT_32(BITS_GET(Operand2, 0, 7));
      int Imm     = bits_get2(Operand2, 0, 7);
      int ramount = bits_get2(Operand2, 8, 11) * 2;
      Operand2    = rotate(Imm, ramount);
      break;
    }
    default : system_exit("If you see this then you really f'd up!");
  }


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
		case 13 : {
      result = Operand2;            break;
      
    } // mov
		default : result = 0;
	}
  
	if (IS_SET(S)) // sets flags
	{
		if (result == 0) CPSRset(ZERO); 		// Z
		CPSRput(NEGATIVE, BIT_GET(result, 31));	// N
    
		switch (OpCode) // It is important to make this unsigned
		{
			case 4  : CPSRput(CARRY, CPSRget(OVERFLOW)); break; // add
			case 2  :
			case 3  :
			case 10 : CPSRput(CARRY, result >= 0); break; // sub, rsb, cmp
		}
	} // end if (S == 1)
  
	switch (OpCode)
	{
		case 8  :
		case 9  :
		case 10 :                              break; // tst, teq, cmp result not written
		default : ARM->registers[Rd] = result; break; // keep result
	}
}

//////////  IMMEDIATE SHIFT REGISTER  //////////////////////////////////////////

int32_t immediate_shifted_register(int32_t word12, int8_t S)
{
	ShiftReg *sreg = (ShiftReg *) &word12;
  
	int flag   = sreg->Flag;
	int amount = 0;
  
	if (IS_CLEAR(flag)) amount = sreg->Amount;
	else
	{
		int rnum = sreg->Amount;
		// BIT_CLEAR(amount, 27); Not needed?
		int Rs = ARM->registers[rnum];
		amount = bits_get2(Rs, 0, 8);
	}
  
	int Type     = sreg->Type;
	int Rm       = sreg->Rm;
	int shiftreg = ARM->registers[Rm];
  
	switch (Type)
	{
		case 0 : // Logical shit left
		{
			word12 = shiftreg << amount;
			int carry = 0;
			if (amount != 0) carry = BIT_GET(shiftreg, 31 - amount + 1);
			if (IS_SET(S)) put_CPSR_flag(CARRY, carry);
			break;
		}
		case 1 : // Logical right shift
		{
			word12 = shiftreg >> amount;
			int carry = 0;
			if (amount != 0) carry = BIT_GET(shiftreg, amount - 1);
			if (IS_SET(S)) put_CPSR_flag(CARRY, carry);
			break;
		}
		case 2 : // Arithmetic right shift
		{
			word12 = shiftreg >> amount;
			int carry = 0;
			if (amount != 0) carry = BIT_GET(shiftreg, amount - 1);
			if (S == 1) put_CPSR_flag(CARRY, carry);
			int bit = BIT_GET(shiftreg, 31); // TODO move to bits set
			for (int j = 0; j < amount; j++) bit_put(word12, 31 - j, bit);
			break;
		}
		case 3 : word12 = rotate(shiftreg, amount); break; //rotate right
		default : system_exit("INVALID INSTRUCTION FORMAT");
	}
  
	return word12;
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
  
	int result = ARM->registers[Rm] * ARM->registers[Rs];
  
	if (IS_CLEAR(A)) ARM->registers[Rd] = result;
	else
	{
		result += ARM->registers[Rn];
		ARM->registers[Rd] = result;
	}
  
	if (IS_SET(S))
	{
		// N is set to bit 31 of the result and
		// Z is set if and only if the result is zero.
		int resbit = BIT_GET(result, 31);
    put_CPSR_flag(NEGATIVE, resbit == 1);
		if (result == 0) CPSRset(ZERO);
	}
}

//////////  BRANCH INSTRUCTION  //////////////////////////////////////////////



void exe_branch(int32_t word)
{
	//BranchInstr *instr = (BranchInstr *) &word;
  
	//int offset = (instr->Offset) << 2;
  
  int offset = bits_get2(word, 0, 23);
  print_bits(offset);
  
  offset <<= 2;
  print_bits(offset);
  
  offset <<= 6;
  print_bits(offset);
  
  offset >>= 6;
  print_bits(offset);
  
  //int what = *(int*)(((char*)&word)+1);
  //print_bits(what);
  
	ARM->registers[PC]    += (offset); // -8 bits ??
  
  ARM->pipeline->fetched = memory_word_read(ARM->registers[PC]);
	ARM->registers[PC] += 4;
	//ARM->pipeline->fetched = ARM->memory[0];
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
	int32_t first  = ARM->memory[m+3]& 0xFF,
  second = ARM->memory[m+2] & 0xFF,
  third  = ARM->memory[m+1] & 0xFF,
  fourth = ARM->memory[m+0] & 0xFF;
  
	return ((first << 24) | (second << 16) | (third << 8) | fourth);
	//return ((fourth << 24) | (third << 16) | (second << 8) | first);
}


int32_t test_glue(int8_t a, int8_t b, int8_t c, int8_t d)
{
  
	return ((a << 24) + (b << 16) + (c << 8) + d);
}

void memory_byte_write(uint16_t memory_address, int8_t byte)
{
  ARM->memory[memory_address] = byte;
}

void memory_word_write(uint16_t memory_address, int32_t word)
{

  int8_t first = (word >> 24) & 0xFF;
  int8_t second = (word >> 16) & 0xFF;
  int8_t third = (word >> 8) & 0xFF;
  int8_t fourth = word & 0xFF;

  memory_byte_write(memory_address, fourth);
  memory_byte_write(memory_address+1, third);
  memory_byte_write(memory_address+2, second);
  memory_byte_write(memory_address+3, first);
}








