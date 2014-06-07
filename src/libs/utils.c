#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

//////////////////////////////////////////////////////////////////  MEMORY CHECK

void *mem_chk(void *p, const char *msg)
{
	if (p != NULL) return p;
	fprintf(stdout, "%s\n", "!MEMORY ERROR!");
	perror(msg);
	exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////////////  FILE CHECK

void file_chk(int predicate, const char *msg)
{
	if (predicate) return;
	fprintf(stdout, "%s\n", "!FILE ERROR!");
	perror(msg);
	exit(EXIT_FAILURE);
}

/////////////////////////////////////////////////////////////////  GENERIC ERROR

void gen_chk(int predicate, const char *msg)
{
	if (predicate) return;
	fprintf(stdout, "%s\n", "!GENERIC ERROR!");
	perror(msg);
	exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////////////////////////
