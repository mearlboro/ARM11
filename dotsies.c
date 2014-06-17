#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>


void initpins(void)
{
    printf("ldr r0, =0x20200000\n");
    
    printf("mov r1, #1\n");
    printf("lsl r1, #12\n");
    printf("str r1, [r0, #0]\n");
    
    printf("mov r2, #1\n");
    printf("lsl r2, #21\n");
    printf("str r2, [r0, #4]\n");
    
    printf("mov r3, #0x1000000\n");
    printf("orr r3, r3, r2\n");
    printf("str r3, [r0, #4]\n");
    
    printf("mov r4, #1\n");
    printf("lsl r4, #6\n");
    printf("str r4, [r0, #8]\n");
    
    printf("mov r5, #0x200000\n");
    printf("orr r5, r5, r4\n");
    printf("str r5, [r0, #8]\n");
    
    
    printf("mov r1, #1\n");
    printf("lsl r1, #4\n");
    
    printf("mov r2, #1\n");
    printf("lsl r2, #17\n");
    
    printf("mov r3, #1\n");
    printf("lsl r3, #18\n");
    
    printf("mov r4, #1\n");
    printf("lsl r4, #22\n");
    
    printf("mov r5, #1\n");
    printf("lsl r5, #27\n");
    
    
    printf("mov r10, #1\n");
    printf("mov r12, #0\n");
    
}


void clearpins(void)
{
    printf("str r1, [r0, #40]\n");
    printf("str r2, [r0, #40]\n");
    printf("str r3, [r0, #40]\n");
    printf("str r4, [r0, #40]\n");
    printf("str r5, [r0, #40]\n");
}


void waitloop(int i, int iter)
{
    printf("mov r9, #0x%X\n", iter);
    printf("waitloop%d:\n", i);
    printf("sub r9, r9, #1\n");
    printf("cmp r9, #0\n");
    printf("bne waitloop%d\n", i);
}

static int dotsies[] =
{
     1 /* a */,  2 /* b */,  4 /* c */,  8 /* d */, 16 /* e */,  3 /* f */,
     6 /* g */, 12 /* h */, 24 /* i */,  5 /* j */, 10 /* k */, 20 /* l */,
     9 /* m */, 18 /* n */, 17 /* o */,  7 /* p */, 11 /* q */, 13 /* r */,
    14 /* s */, 22 /* t */, 26 /* u */, 28 /* v */, 19 /* w */, 21 /* x */,
    25 /* y */, 27 /* z */,  0 /* space */
};


int main(int argc, const char **argv)
{
    freopen(argv[1], "r", stdin);  // Input file
    freopen(argv[2], "w", stdout); // Output file
    
    int iter = 0xFF0000; // Set up iteration speed
    if (argc == 4)
    {
        iter = atoi(argv[3]);
        iter = (iter < 1 || iter > 10) ? 0xFF0000 : (0xFF0000 / iter);
    }

    initpins();
    
    char letter;
    for (int i = 0; scanf("%c", &letter) != EOF; i+=2)
    {
        int c = tolower(letter) - 'a';
        if (c < 0 || c > 25) c = 26; // Non-alphabetical characters are spaces

        clearpins();
        waitloop(i, iter);
        
        unsigned int mask = 1;
        for (int b = 1; b < 6; b++)
        {
            if ((dotsies[c] & (mask << (b-1))) == (1 << (b-1)))
            {
                // Light up pins according to Doties equivalent
                printf("str r%d, [r0, #28]\n", b);
            }
        }
        
        waitloop(i+1, iter);
    }
    return 0;
}


