#ifndef _INSTRUCTION_TYPE
#define _INSTRUCTION_TYPE

////////////////////////////////////////////////////////////////////////////////
////  2. INSTRUCTION DEFINITIONS  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////  2.1.0 INSTRUCTION TYPES //////////////////////////////////////////////////

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


////  2.1.1 SHIFT TYPES ///////////////////////////////////////////////////////

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


////  2.1.2 OPCODES DEFINITION /////////////////////////////////////////////////

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
} DataProcessingOpcodes;

////  2.2 CONDITION CODE DEFINITION  //////////////////////////////////////////

typedef enum 
{
	EQ =  0,
	NE =  1,
	GE = 10,
	LT = 11,
	GT = 12,
	LE = 13,
	AL = 14
} ConditionCode;



#endif
