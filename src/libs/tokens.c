#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "utils.h"

/////////////////////////////////////////////////////////////////////////  FREE

void toks_free(tokens *toks)
{
	for (int i = 0; i < toks->tokn; i++)
	{
		free(toks->toks[i]);
	}
	free(toks->toks);
	free(toks->line);
	free(toks);
}

////////////////////////////////////////////////////////////////////////  PRINT

void toks_print(tokens *toks)
{
	printf("Printing %i tokens:\n", toks->tokn);
	for (int i = 0; i < toks->tokn; i++)
	{
		printf("%s\n", toks->toks[i]);
	}
}

//////////////////////////////////////////////////////////////////////  TOKENIZE

static tokens *toks_new()
{
	tokens *toks = mem_chk(malloc(sizeof(tokens)));
	toks->toks   = mem_chk(malloc(0));
	return toks;
}

tokens *tokenize(char *str, const char *delim)
{
	tokens *toks = toks_new();
	char   *tok  = NULL;
	int     tokn = 0;
	size_t  tokz = 0;

	toks->line = strdup(str);

	/* TODO: Y U NO?
	while ((tok = strsep(&str, delim)) != NULL)
	{
		if (*tok == '\0') continue; // Discard empty tokens

		tokz               = sizeof(char *) * (tokn + 1);
		toks->toks         = mem_chk(realloc(toks->toks, tokz));
		toks->toks[tokn++] = strdup(tok);
	}
	toks->tokn = tokn;*/

    tok = strtok(str, delim);
	while (tok != NULL)
	{
		if (*tok == '\0') continue; // Discard empty tokens

		tokz               = sizeof(char *) * (tokn + 1);
		toks->toks         = mem_chk(realloc(toks->toks, tokz));
		toks->toks[tokn++] = strdup(tok);
		tok = strtok(0, delim);
	}
	toks->tokn = tokn;

	return toks;
}

///////////////////////////////////////////////////////////////////////  ITERATE

void toks_iter(tokens *toks, toks_func func)
{
	for (int i = 0; i < toks->tokn; i++)
	{
		func(toks->toks[i]);
	}
}

/////////////////////////////////////////////////////////////////////  LAST CHAR

char toks_endc(tokens *toks)
{
	return toks->toks[toks->tokn-1][strlen(toks->toks[toks->tokn-1])-1];
}

