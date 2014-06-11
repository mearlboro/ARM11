#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "utils.h"

///////////////////////////////////////////////////////////////////////////  NEW

static tokens *toks_new()
{
	tokens *toks = malloc(sizeof(tokens));
	toks->toks   = malloc(0);
	return toks;
}

//////////////////////////////////////////////////////////////////////////  FREE

void toks_free(tokens *toks)
{
	free(toks);
	free(toks->toks);
}

/////////////////////////////////////////////////////////////////////////  PRINT

void toks_print(tokens *toks)
{
	printf("Printing %i tokens:\n", toks->tokn);
	for (int i = 0; i < toks->tokn; i++)
	{
		printf("%s\n", toks->toks[i]);
	}
}

//////////////////////////////////////////////////////////////////////  TOKENIZE

tokens *tokenize(char *str, const char *delim)
{
	tokens *toks = toks_new();
	char   *tok  = NULL;
	int     tokn = 0;
	size_t  tokz = 0;
	
	toks->line = strdup(str);
	
	while ((tok = strsep(&str, delim)) != NULL)
	{
		if (*tok == '\0') continue; // Discard empty tokens
		
		tokz               = sizeof(char *) * (tokn + 1);
		toks->toks         = realloc(toks->toks, tokz);
		toks->toks[tokn++] = strdup(tok);
	}
	toks->tokn = tokn;
	
	return toks;
}


///////////////////////////////////////////////////////////////////////  ITERATE

void toks_iter(tokens *toks, toks_fun fun)
{
	for (int i = 0; i < toks->tokn; i++)
	{
		fun(toks->toks[i]);
	}
}

/////////////////////////////////////////////////////////////////////  LAST CHAR

char toks_endc(tokens *toks)
{
	return toks->toks[toks->tokn-1][strlen(toks->toks[toks->tokn-1])-1];
}

//////////////////////////////////////////////////////////////////////  TOKENIZE

/*Tokens **tokenize_r(Tokens *tokens, const char *delim)
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
}*/

