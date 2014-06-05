#include <stdio.h>
#include <string.h>

/*#define mklist(f) \
    f(ADD) f(EOR) f(SUB)

#define f_enum(x) x,
#define f_arr(x) {x, #x},

enum Opcodes { mklist(f_enum) OP_NUM };

struct { enum Opcodes op; char * str; } oparr[] = { mklist(f_arr) };*/

#define enum_string(f) \
	f(AND) f(EOR) f(SUB) f(RSB) f(ADD) f(TST) f(TEQ) f(CMP) f(ORR) f(MOV) f(MUL) f(MLA) f(BEQ) f(BNE) f(BGE) f(BLT) f(BGT) f(BLE) f(B) f(LDR) f(STR) f(LSL) f(ANDEQ)

#define f_enum(x) x,
#define f_array(x) {x, #x},

enum Opcode { enum_string(f_enum) OPCODE_ENUM };

struct { enum Opcode op; char * str; } opcode_array[] = { enum_string(f_array) };

int main(int argc, char* argv[]) {
    int i;
    for (i=0; i < sizeof(opcode_array)/sizeof(opcode_array[0]); i++) {
        if (strcmp(argv[1], opcode_array[i].str) == 0) {
            printf("%d %s\n", opcode_array[i].op, opcode_array[i].str);
            return 0;
        }
    }
    printf("not found\n");
    return 1;
}
