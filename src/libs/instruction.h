#ifndef _INSTRUCTION_TYPE
#define _INSTRUCTION_TYPE

#include <stdint.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
////  EMULATOR: INSTRUCTION DEFINITIONS  ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////  INSTRUCTION TYPES ///////////////////////////////////////////////////////

typedef struct DataProcessingInstr
{
	unsigned int Operand2 : 12;
	unsigned int Rd       : 4;
	unsigned int Rn       : 4;
	unsigned int S        : 1;
	unsigned int OpCode   : 4;
	unsigned int _I        : 1;
	unsigned int _00      : 2;
	unsigned int Cond     : 4;
} DataProcessingInstr;

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
	unsigned int _I      : 1;
	unsigned int _01    : 2;
	unsigned int Cond   : 4;
} SingleDataTransferInstr;

typedef struct BranchInstr
{
	unsigned int Offset : 24;
	unsigned int _1010  : 4;
	unsigned int Cond   : 4;
} BranchInstr;


////  SHIFT TYPES /////////////////////////////////////////////////////////////

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

////  MNEMONICS DEFINITIONS ////////////////////////////////////////////////////

typedef enum 
{
	AND =  0,
	EOR =  1,
	SUB =  2,
	RSB =  3,
	ADD =  4,
	TST =  8,
	TEQ =  9,
	CMP = 10,
	ORR = 12,
	MOV = 13,
} Opcode;

typedef enum
{
	EQ =  0,
	NE =  1,
	GE = 10,
	LT = 11,
	GT = 12,
	LE = 13,
	AL = 14
} CondCode;

typedef enum 
{
	ASR = 2,
	ASL = 0,
	LSL = 0,
	LSR = 1,
	ROR = 3
} ShiftType;


////////////////////////////////////////////////////////////////////////////////
////  ASSEMBLER: STRING TO MNEMONIC  ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define f_pair(x) {x, #x},

void to_lowercase(char *x) 
{ 
	for (int i=0; i<strlen(x); i++) 
		if(x[i] > 'z' || x[i] < 'a') x[i] |= 0x20; 
} 


////  MNEMONIC ///////////////////////////////////////////////////////////////////
#define mnemonic_tostring(f) \
	f(and) f(eor) f(sub) f(rsb) f(add) f(tst) f(teq) f(cmp) f(orr) f(mov) f(mul) \
	f(mla) f(beq) f(bne) f(bge) f(blt) f(bgt) f(ble) f(b) f(ldr) f(str) f(lsl) f(andeq)

enum Mnemonic { and, eor = 0, sub = 0, rsb = 0, add = 0,
		tst, teq = 1, cmp = 1, orr = 1,
		mov, 
		mul, mla,
		beq, bne = 5, bge = 5, blt = 5, bgt = 5, ble = 5, b = 5,
		ldr, str = 6, 
		lsl, andeq };                             
		

struct { enum Mnemonic no; char * str; } 
	mnemonic_array[] = { mnemonic_tostring(f_pair) };


////  DATA PROCESSING OPCODE 
#define opcode_tostring(f) \
	f(AND) f(EOR) f(SUB) f(RSB) f(ADD) f(TST) f(TEQ) f(CMP) f(ORR) f(MOV)

struct { Opcode no; char * str; } 
	opcode_array[] = { opcode_tostring(f_pair) };


////  BRANCH CONDITION CODE 
#define cond_tostring(f) \
	f(EQ) f(NE) f(GE) f(LT) f(GT) f(LE) f(AL)

struct { CondCode no; char * str; } 
	cond_array[] = { cond_tostring(f_pair) };


////  SHIFT TYPE  
#define shift_tostring(f) \
	f(ASR) f(ASL) f(LSL) f(LSR) f(ROR)

struct { ShiftType no; char * str; } 
	shift_array[] = { shift_tostring(f_pair) };


//// GET ENUM FROM STRING FUNCTION ////////////////////////////////////////////



////  FOR ASSIGNING MNEMOTIC TO FUNCTION


#define STR_TO_ENUM(a) int str_to_##a(char *x) \
{ \
	int no_enums = sizeof(a##_array)/sizeof(a##_array[0]); \
	for (int i = 0; i < no_enums; i++) \
	{	\
		char *low_x = to_lowercase( a##_array[i].str); \
		if (!strcmp(x, low_x)) \
			return a##_array[i].no; \
	} \
	return 100; \
} 

/*
STR_TO_ENUM(mnemonic);
STR_TO_ENUM(opcode);
STR_TO_ENUM(cond);
STR_TO_ENUM(shift); 
*/

int str_to_mnemonic(char *x)
{
	int no_enums = sizeof(mnemonic_array)/sizeof(mnemonic_array[0]);
	for (int i = 0; i < no_enums; i++) 
	{	
		char *low_x = mnemonic_array[i].str;
		to_lowercase(low_x);
		if (!strcmp(x, low_x))
			return mnemonic_array[i].no;
	}
	return 100;
}

////  FOR USE IN INSTRUCTIONS
int str_to_opcode(char *x)
{
	int no_enums = sizeof(opcode_array)/sizeof(opcode_array[0]);
	for (int i = 0; i < no_enums; i++) 
	{
		char *low_x = mnemonic_array[i].str;
		to_lowercase(low_x);
		if (!strcmp(x, low_x))
			return opcode_array[i].no;
	}
	return 100;
}


int str_to_cond(char *x)
{
	int no_enums = sizeof(cond_array)/sizeof(cond_array[0]);
	for (int i = 0; i < no_enums; i++) 
	{
		char *low_x = mnemonic_array[i].str;
		to_lowercase(low_x);
		if (!strcmp(x, low_x))
			return cond_array[i].no;
	}
	return 100;
}
	
int str_to_shift(char *x)
{
	int no_enums = sizeof(shift_array)/sizeof(shift_array[0]);
	for (int i = 0; i < no_enums; i++) 
	{
		char *low_x = mnemonic_array[i].str;
		to_lowercase(low_x);
		if (!strcmp(x, low_x))
			return shift_array[i].no;
	}
	return 100;
}	



#endif
