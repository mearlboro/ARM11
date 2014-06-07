#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

////  CONDITION CODE //////////////////////////////////////////////////////////
/*#define cond_tostring(f) \
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
}*/


typedef enum 
{
	eq =0,
	ne,
	ge = 10,
	lt,
	gt,
	le,
	al
} ConditionCode;

#define NUM_CONDITION_CODES 7

#define STR(x) #x



int main(int argc, char **argv)
{
	char *opcode = argv[1];
	for (int i = 0; i < NUM_CONDITION_CODES; i++)
	{                           
		if (strcmp(opcode, opcode_array[i].str) == 0)                           
		{                 
			function_array[opcode_array[i].op](tokens);
		}                                                 
	}       


}
