#ifndef _OPCODE_TYPE
#define _OPCODE_TYPE

////////////////////////////////////////////////////////////////////////////////
////  6. OPCODE DEFINITIONS  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define enum_string(f) \
	f(AND) f(EOR) f(SUB) f(RSB) f(ADD) f(TST) f(TEQ) f(CMP) f(ORR) f(MOV) f(MUL) \
	f(MLA) f(BEQ) f(BNE) f(BGE) f(BLT) f(BGT) f(BLE) f(B) f(LDR) f(STR) f(LSL) f(ANDEQ)

#define f_enum(x) x,
#define f_array(x) {x, #x},

enum Opcode { enum_string(f_enum) OPCODE_ENUM };

struct { enum Opcode op; char * str; } opcode_array[] = { enum_string(f_array) };


#endif
