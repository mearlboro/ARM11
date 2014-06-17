#ifndef _INSTRUCTION_TYPE
#define _INSTRUCTION_TYPE

#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"

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
	unsigned int _I       : 1;
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
	unsigned int _I     : 1;
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


typedef struct ShiftRegOptional
{
	unsigned int Amount : 5;
	unsigned int Type   : 2;
	unsigned int Flag   : 1;
	unsigned int Rm     : 4;
} ShiftRegOptional;

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
	ASL = 0,
	LSL = 0,
	LSR = 1,
	ASR = 2,
	ROR = 3
} ShiftType;


////////////////////////////////////////////////////////////////////////////////
////  ASSEMBLER: STRING TO MNEMONIC  ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define f_pair(x) {x, #x },

////  MNEMONIC ///////////////////////////////////////////////////////////////////
#define mnemonic_tostring(f) \
	f(and) f(eor) f(sub) f(rsb) f(add) f(tst) f(teq) f(cmp) f(orr) f(mov) f(mul) \
	f(mla) f(beq) f(bne) f(bge) f(blt) f(bgt) f(ble) f(b) f(ldr) f(str) f(lsl) \
	f(andeq)

enum Mnemonic { and, eor = 0, sub = 0, rsb = 0, add = 0, orr = 0,
                tst, teq = 1, cmp = 1, 
                mov,
                mul, mla,
                beq, bne = 5, bge = 5, blt = 5, bgt = 5, ble = 5, b = 5,
                ldr, str = 6,
                lsl, andeq };


struct { enum Mnemonic no; char * str; }
         mnemonic_array[] = { mnemonic_tostring(f_pair) };


////  DATA PROCESSING OPCODE  //////////////////////////////////////////////////
#define opcode_tostring(f) \
	f(AND) f(EOR) f(SUB) f(RSB) f(ADD) f(TST) f(TEQ) f(CMP) f(ORR) f(MOV)

struct { Opcode no; char * str; }
         opcode_array[] = { opcode_tostring(f_pair) };


////  BRANCH CONDITION CODE  ///////////////////////////////////////////////////
#define cond_tostring(f) \
	f(EQ) f(NE) f(GE) f(LT) f(GT) f(LE) f(AL)

struct { CondCode no; char * str; }
         cond_array[] = { cond_tostring(f_pair) };


////  SHIFT TYPE  //////////////////////////////////////////////////////////////
#define shift_tostring(f) \
	f(ASL) f(LSL) f(LSR) f(ASR) f(ROR)

struct { ShiftType no; char * str; }
         shift_array[] = { shift_tostring(f_pair) };



////  GET ENUM FROM STRING FUNCTION  ///////////////////////////////////////////

/* 
 * Converts a string to lowercase if there are any uppercase characters
 * the string remains unchanged if it is lowercase
 */
static char *strtolwr(char *buffer)
{
	char *str = strdup(buffer);

	for (int i=0; i < strlen(str); i++)
	  if (str[i] > 'z' || str[i] < 'a') str[i] = tolower(str[i]);

	return str;
}


#define STR_TO_ENUM(a) int str_to_##a(char *x)           \
{                                                        \
	int len = sizeof(a##_array)/sizeof(a##_array[0]);      \
	for (int i = 0; i < len; i++)                          \
	{	                                                     \
		char *low_x = strtolwr(a##_array[i].str);            \
		if (strcmp(x, low_x) == 0)                           \
		{                                                    \
			return a##_array[i].no;                            \
		}                                                    \
	}                                                      \
	return -1;                                             \
}


STR_TO_ENUM(mnemonic)
STR_TO_ENUM(opcode)
STR_TO_ENUM(cond)
STR_TO_ENUM(shift)



#endif
