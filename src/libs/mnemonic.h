#ifndef _MNEMONIC_TYPE
#define _MNEMONIC_TYPE

////////////////////////////////////////////////////////////////////////////////
////  6. MNEMONIC DEFINITIONS  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define f_enum(x) x,
#define f_pair(x) {x, #x},

void to_lowercase(char *x) 
{ 
	for (int i=0; i<strlen(x); i++) 
		if(x[i] > 'z' || x[i] < 'a') x[i] |= 0x20; 
} 


////  MNEMONIC /////////////////////////////////////////////////////////////////
#define mnemonic_tostring(f) \
	f(and) f(eor) f(sub) f(rsb) f(add) f(tst) f(teq) f(cmp) f(orr) f(mov) f(mul) \
	f(mla) f(beq) f(bne) f(bge) f(blt) f(bgt) f(ble) f(b) f(ldr) f(str) f(lsl) \
	f(andeq)

enum Mnemonic {   and, eor = 0, sub = 0, rsb = 0, add = 0,
		tst, teq = 1, cmp = 1, orr = 1,
		mov, 
		mul, mla,
		beq, bne = 5, bge = 5, blt = 5, bgt = 5, ble = 5, b = 5,
		ldr, str = 6, 
		lsl, andeq };                             
		

struct { enum Mnemonic no; char * str; } 
	mnemonic_array[] = { mnemonic_tostring(f_pair) };


////  CONDITION CODE //////////////////////////////////////////////////////////
#define cond_tostring(f) \
	f(eq) f(ne) f(ge) f(lt) f(gt) f(le) f(al)

enum ConditionCode { eq = 0, ne, ge = 10, lt, gt, le, al };

struct { enum ConditionCode no; char * str; } 
	cond_array[] = { cond_tostring(f_pair) };


////  SHIFT TYPE  /////////////////////////////////////////////////////////////
#define shift_tostring(f) \
	f(ASR) f(ASL) f(LSL) f(LSR) f(ROR)

enum ShiftType
{
	ASR = 2,
	ASL = 0,
	LSL = 0,
	LSR = 1,
	ROR = 3
};

struct { enum ShiftType no; char * str; } 
	shift_array[] = { shift_tostring(f_pair) };



//// GET ENUM FROM STRING FUNCTION ////////////////////////////////////////////
int str_to_mnemonic(char *x)
{
	int no_enums = sizeof(mnemonic_array)/sizeof(mnemonic_array[0]);
	for (int i = 0; i < no_enums; i++) 
	{	
		char *low_x = mnemonic_array[i].str;
		to_lowercase(low_x);
		if (!strcmp(x, mnemonic_array[i].str))
			return mnemonic_array[i].no;
	}
	return 100;
}



int str_to_cond(char *x)
{
	int no_enums = sizeof(cond_array)/sizeof(cond_array[0]);
	for (int i = 0; i < no_enums; i++) 
		if (!strcmp(x, cond_array[i].str)) 
			return cond_array[i].no;
	return 100;
}
	
int str_to_shift(char *x)
{
	int no_enums = sizeof(shift_array)/sizeof(shift_array[0]);
	for (int i = 0; i < no_enums; i++) 
		if (!strcmp(x, shift_array[i].str)) 
			return shift_array[i].no;
	return 100;
}	


#endif
