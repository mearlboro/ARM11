#ifndef _MNEMONIC_TYPE
#define _MNEMONIC_TYPE

////////////////////////////////////////////////////////////////////////////////
////  6. MNEMONIC DEFINITIONS  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define f_enum(x) x,
#define f_pair(x) {x, #x},


////  OPCODE ///////////////////////////////////////////////////////////////////
#define opcode_tostring(f) \
	f(and) f(eor) f(sub) f(rsb) f(add) f(tst) f(teq) f(cmp) f(orr) f(mov) f(mul) \
	f(mla) f(beq) f(bne) f(bge) f(blt) f(bgt) f(ble) f(b) f(ldr) f(str) f(lsl) f(andeq)

enum Opcode { opcode_tostring(f_enum) OPCODE_ENUM };

struct { enum Opcode opcode; char * str; } 
	opcode_array[] = { opcode_tostring(f_pair) };


////  CONDITION CODE //////////////////////////////////////////////////////////
#define cond_tostring(f) \
	f(eq) f(ne) f(ge) f(lt) f(gt) f(le) f(al)

enum ConditionCode { eq = 0, ne, ge = 10, lt, gt, le, al };

struct { enum ConditionCode cond; char * str; } 
	cond_array[] = { cond_tostring(f_pair) };


//// GET ENUM FROM STRING FUNCTION ////////////////////////////////////////////

int str_to_opcode(char *x)
{
	int no_enums = sizeof(opcode_array)/sizeof(opcode_array[0]);
	for (int i = 0; i < no_enums; i++) 
		if (!strcmp(x, opcode_array[i].str)) 
			return opcode_array[i].opcode;
	return 0;
}

int str_to_cond(char *x)
{
	int no_enums = sizeof(cond_array)/sizeof(cond_array[0]);
	for (int i = 0; i < no_enums; i++) 
		if (!strcmp(x, cond_array[i].str)) 
			return cond_array[i].cond;
	return 0;
}
	
	


#endif
