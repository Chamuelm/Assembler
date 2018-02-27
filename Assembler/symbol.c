#define SYMBOL_C
#include "assembler.h"
#include <stdlib.h>
#include <stdio.h>

symbol *symTable[HASHSIZE];		/* Symbols table */

/* hash:	form hash value for string s */
unsigned int hash(char *s)
{
	unsigned int hashval;
	
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31*hashval;
	return hashval % HASHSIZE;
}

/*	symLookup: look for s in symTable */
symbol *symLookup(char *s)
{
	symbol *np;

	for(np = symTable[hash(s)]; np != NULL; np = np->next)
		if(strcmp(s, np->name) == 0)
			return np;
	return NULL;
}

/* addSymbol:	add new symbol to symbols table */
symbol *addSymbol(char *newName, int newAddress, enum lineTypes newType)
{
	symbol *np;
	unsigned hashval;

	if((np = symLookup(newName)) == NULL)	/* Check if symbol already exist */
	{
		np = (symbol *) malloc(sizeof(*np));
		if(np == NULL || (np->name = strdup(newName)) == NULL)	/* Successful alloc? */
			exitMemory();	/* Exit if memory allocation failed */
		
		/* Put in symbol table by hash */
		hashval = hash(name);
		np->next = symTable[hashval];
		np->address = newAddress;
		np->type = newType;
		symTable[hashval] = np;
	}
	else						/* Symbol already exist! */
		return NULL;
	return np;
}

/* strdup: Copy string s to returned new allocated space */

char *strdup(char *s)
{
  char *p;
  p = (char *) malloc(strlen(s) + 1);
  if(p != NULL)
    strcpy(p, s);
  return p;
}

/*	Initialize symbol table */
void symTableInit()
{
	int i;

	for(i=0; i < HASHSIZE; i++
	{
			if(symTable[i] != NULL)
			{
				freeSymbol(symTable[i]);
				symTable[i] = NULL;
			}
	}
}

/* freeSymbol:	free symbol and all symbols attached to it. */
void freeSymbol(symbol *s)
{
	if (s->next != NULL)		/* If chained to more symbols recursivly free them */
		freeSymbol(s->next);
	free(s->name);					/* Free name pointer */
	free(s);								/* Free symbol pointer */
}
