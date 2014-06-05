#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.c"

//////////////////////////////////////////////////////////////////////

typedef struct Tokens
{
	const char **toks;
	unsigned int tokno;
} Tokens;


Tokens *toks_new() 
{	
	Tokens *tokens = NULL;
	ERR_MEM((tokens       = malloc(sizeof(Tokens)))  == NULL);
	ERR_MEM((tokens->toks = malloc(sizeof(char **))) == NULL);
	return tokens;
}

void toks_free(Tokens *tokens)
{
	free(tokens);
	free(tokens->toks);
}


void toks_print(Tokens *tokens)
{
	printf("Tokens:\n");
	for (int i = 0; i < tokens->tokno; i++) 
	{
		printf("%s\n", tokens->toks[i]);
	}
	printf("Token count: %i\n", tokens->tokno);
}


Tokens *tokenize(char *str, const char *delim)
{
	Tokens *tokens = toks_new();
	for (int n = 0; ; n++)
	{
		if (n != 0) str = NULL;
		char *token = strtok(str, delim);
		if (token == NULL) {
			tokens->tokno = n;
			break;
		} 
		tokens->toks[n] = token;
	}
	return tokens;
}

//////////////////////////////////////////////////////////////////////

void read_assembly_program(const char *filepath)
{
	FILE *file = fopen(filepath, "rt");

	ERR_FILE(file == NULL); 	
	ERR_FILE(fseek(file, 0, SEEK_END) == -1);

	long bytes = ftell(file);
	rewind(file);
			
	char program[bytes];
	ERR_FILE(fread(program, 1, bytes, file) != bytes);

	Tokens *instructions = tokenize(program, "\n");
	toks_print(instructions);
}


//char *strsep(char **stringp, const char *delim);

int main(int argc, char **argv) 
{

	read_assembly_program(argv[1]);

  return EXIT_SUCCESS;
}




























































/*

int main(int argc, char **argv) {


	ListNode * head = malloc(sizeof(ListNode));
	head->data = 4;
	head->next = NULL;	
	head=list_insert(head,13);
	list_print(head);
	head=list_insert(head,4);
	list_print(head);
	head=list_insert(head,6);
	list_print(head);
	head=list_insert(head,10);
	list_print(head);
	head=list_remove(head,6);
	list_print(head);
	head=list_remove(head,4);
	printf("%d", (list_search(head, 4) == NULL) ? 0 : 1);

	SymbolTable *new = symtable_init();

	symtable_put(new,"lala",0x99);
	symtable_print(new);
	symtable_remove(new, "lala");
	symtable_print(new);

  return EXIT_SUCCESS;
}

*/
