#include <stdlib.h>
#include <stdint.h>

// ARM main memory has 2^16 = 65536 bytes
#define MEMORY_CAPACITY 65536

// ARM has 17, 32-bit registers
#define REGISTER_COUNT 17

// This represents the state of an ARM machine: main memory and registers
typedef struct ARM_state
{
  int8_t memory[MEMORY_CAPACITY];
  int32_t registers[REGISTER_COUNT];
} ARM;

// This represets the three-stage fetch/decode/execute pipeline
typedef struct pipeline 
{
  int32_t fetched;
  int32_t decoded;
  //int32_t executed;
} pipeline;



int main(int argc, char **argv) 
{
  // Obtain binary input file path
  
  // Initialize ARM's main memory and registers to 0
    
  // Read binary input file (the program) and load it into ARM's main memory
  // ON ERROR: notify user something went wrong :(
  // ON SUCCESS: continue

  // Fetch instrction at memory[0]. That is the inital value of PC
  
  // Load initial PC value into registers[PC]
  
  // Begin emulator_loop()

}


// THREE-STAGE EXECUTION PIPELINE
void emulator_loop()
{

  // FETCH NEXT INSTRUCTION
  // pipeline fetched <-- [PC]
  
  // INCREMENT PROGRAM COUNTER
  // PC = PC + 4
  
  
  /*** main_loop ***/
  // DECODE FETCHED INSTRUCTION
  // pipeline decoded <-- pipeline fetched
  
  // FETCH NEXT INSTRUCTION
  // pipeline fetched <-- [PC]
  
  // INCREMENT PROGRAM COUNTER
  // PC = PC + 4
  
  // EXECUTE CURRENT DECODED INSTRUCTION
  // execute_instruction(pipeline decoded)
  /*** goto main_loop ***/
 
}


void execute_instruction(int32_t word)
{  
  // Check instruction (word)
  // IF: is HALT (32 zeros) THEN terminate and output_ARM_state()
  // ELSE execute it
  
  /*
  
  word[n] means the nth bit of word
  
  ########################################################
  consider word[27] and word[26]
  
  if      01 then exe_single_data_transfer(word)
  
  else if 10 then exe_branch(word)
  
  else if 00 then {
  	
  	if      word[25] == 1 then exe_data_processing(word [operand2 is a shifted register])
  	 
  	else if word[25] == 0 then {
  	
  	  if      word[4] == 0 then exe_data_processing(word)
  	  
  	  else if word[4] == 1 then {
    	  
    	if      word[7] == 1 then exe_multiply(word)
    	
    	else if word[7] == 0 then exe_data_processing(word)
  	  
  	  }
  	  
  	} 
  		
  }
  	
  else ERROR_ERROR_EXTERMINATE_EXTERMINATE!!!
  ########################################################
   
   */
}


void exe_single_data_transfer(int32_t word) {}
void exe_data_processing(int32_t word) {}
void exe_multiply(int32_t word) {}
void exe_branch(int32_t word) {}


void optput_ARM_state()
{
  // Output content of registers
	
  // Oputput content of non-zero memory
}






















