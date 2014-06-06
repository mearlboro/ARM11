#ifndef _OPCODE_TYPE
#define _OPCODE_TYPE

////////////////////////////////////////////////////////////////////////////////
////  6. OPCODE DEFINITIONS  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////  OPCODE ///////////////////////////////////////////////////////////////////
#define opcode_tostring(f) \
	f(and) f(eor) f(sub) f(rsb) f(add) f(tst) f(teq) f(cmp) f(orr) f(mov) f(mul) \
	f(mla) f(beq) f(bne) f(bge) f(blt) f(bgt) f(ble) f(b) f(ldr) f(str) f(lsl) f(andeq)

#define op_enum(x) x,
#define op_array(x) {x, #x},

enum Opcode { opcode_tostring(op_enum) OPCODE_ENUM };

struct { enum Opcode op; char * str; } opcode_array[] = { opcode_tostring(op_array) };


////  CONDITION CODE //////////////////////////////////////////////////////////
#define cond_tostring(f) \
	f(eq) f(ne) f(ge) f(lt) f(gt) f(le) f(al)

#define cond_enum(x) x,
#define cond_array(x) {x, #x},

enum ConditionCode { cond_tostring(cond_enum) COND_ENUM };

struct { enum ConditionCode cond; char * str; } cond_array[] = { cond_tostring(cond_array) };


void set_condition_value() {
	cond_array[0].cond= 0;
	cond_array[1].cond= 1;
	cond_array[2].cond=10;
	cond_array[3].cond=11;
	cond_array[4].cond=12;	
	cond_array[5].cond=13;
	cond_array[6].cond=14;
}

#endif
