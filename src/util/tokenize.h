#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	char **toks;
	unsigned int tokno;
} Tokens;


Tokens *toks_new()
{
	Tokens *tokens = malloc(sizeof(Tokens));
	//tokens->toks = malloc(0);//sizeof(char **));
	return tokens;
}


void toks_free(Tokens *tokens)
{
	free(tokens);
	for (int i = 0; i < tokens->tokno; i++)
	{
		//free(&(tokens->toks[i]));
	}
	free(tokens->toks);
}


void toks_print(Tokens *tokens)
{
	printf("Printing %i tokens:\n", tokens->tokno);
	for (int i = 0; i < tokens->tokno; i++)
	{
		printf("%s\n", tokens->toks[i]);
	}
}


Tokens *tokenize(char *str, const char *delim)
{
	Tokens *tokens  = toks_new();
	char   *token   = NULL;
	int     tokno   = 0;
	size_t  toksize = 0;
	
	while ((token = strsep(&str, delim)) != NULL)
	{
		toksize							  = sizeof(char *) * (tokno + 1);
		tokens->toks					= realloc(tokens->toks, toksize);
		tokens->toks[tokno++] = strdup(token);
	}
	tokens->tokno = tokno;
	
	return tokens;
}


Tokens **tokenize_r(Tokens *tokens, const char *delim)
{
	Tokens **tokarr   = NULL;
	int			 tokarrno = 0;
	size_t   tokarrsz = 0;
	
	for (int i = 0; i < tokens->tokno; i++)
	{
		tokarrsz				   = sizeof(Tokens *) * (tokarrno + 1);
		tokarr             = realloc(tokarr, tokarrsz);
		tokarr[tokarrno++] = tokenize(tokens->toks[i], delim);
	}
	tokarr[0]->tokno = tokarrno;
	
	return tokarr;
}
