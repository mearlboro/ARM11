#ifndef TOKENS_H
#define TOKENS_H

////////////////////////////////////////////////////////////////////////////////

typedef struct tokens
{
	char **toks;
	unsigned int tokn;
} tokens;

typedef void (*toks_fun)(char *);

////////////////////////////////////////////////////////////////////////////////

//static tokens *toks_new();

////////////////////////////////////////////////////////////////////////////////

void toks_free(tokens *);

void toks_print(tokens *);

tokens *tokenize(char *, const char *);

void toks_iter(tokens *, toks_fun);

//  tokens **tokenize_r(tokens *, const char *);

////////////////////////////////////////////////////////////////////////////////

#endif
