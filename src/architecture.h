////////////////////////////////////////////////////////////////////////////////
////  2. INSTRUCTION DEFINITIONS  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef uint16_t ADDRESS;

typedef int32_t WORD;

#define WORD_SIZE 4

////  2.1.0 INSTRUCTION TYPES //////////////////////////////////////////////////

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

////  2.1.2 DATA PROCESSING OPCODES DEFINITION ////////////////////////////////

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
	MOV = 13
} DataProcessingOpCodes;

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
} BranchCondCode;

////  1.3 REGISTER DEFINITIONS  ////////////////////////////////////////////////

typedef enum
{
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12,
	SP, LR,
	PC,
	CPSR
} Register;

////  1.4 CPSR FLAG DEFINITIONS  ///////////////////////////////////////////////

typedef enum
{
	NEGATIVE = 31,
	ZERO 	   = 30,
	CARRY 	 = 29,
	OVERFLOW = 28
} CPSRFlag;


typedef enum
{
	ASR = 2,
	ASL = 0,
	LSL = 0,
	LSR = 1,
	ROR = 3
} ShiftType;


