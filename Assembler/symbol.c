/* 
 * File:    symbol.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains symbols table declaration and functions related.
 * Symbols table is a hash table with the size HASHSIZE which is
 * declared in assembler.h file
 * 
 */
#define SYMBOL_C
#include "./include/assembler.h"
#include <stdlib.h>
#include <string.h>


void freeSymbol(symbol *s);
void addressUpdate(symbol *s, int x);
void printSym(symbol *sym);

symbol *symTable[HASHSIZE];		/* Symbols table */

/*	symLookup: look for s in symTable */
symbol *symLookup(char *s) {
    symbol *np;
    
    for(np = symTable[hash(s)]; np != NULL; np = np->next)
        if(strcmp(s, np->name) == 0)
            return np;
    return NULL;
}

/* addSymbol:	add new symbol to symbols table */
symbol *addSymbol(char *newName, int newAddress, enum lineTypes newType ) {
    symbol *np;
    unsigned hashval;
    
    if((np = symLookup(newName)) == NULL) {	/* Check if symbol already exist */
        np = (symbol *) malloc(sizeof(symbol));
        if(np == NULL)	/* Successful alloc? */
            exitMemory();	/* Exit if memory allocation failed */
        
        /* Put in symbol table by hash */
        hashval = hash(newName);
        np->next = symTable[hashval];
        np->address = newAddress;
        np->type = newType;
        np->isEntry = FALSE;
        strcpy(np->name, newName);
        symTable[hashval] = np;
        
    } else	/* Symbol already exist! */
        return NULL;
    return np; /* symbol added fine */
}

/* removeSymbolPtr: remove sym from symbols table */
void removeSymbol(symbol *toRemove) {
    int h;
    symbol *prev, *np;
    
    prev = NULL;
    h = hash(toRemove->name);
    
    /* lookup in symbol table to find previous */
    for(np = symTable[h]; np!=NULL; np = np->next) {
        if(np == toRemove)
            break;
        prev = np;
    }
    
    if(np !=NULL) {
        if(prev == NULL)
            symTable[h] = np->next;
        else
            prev->next = np->next;
        
        /* set next to NULL for freeSymbol won't free all chain */
        toRemove->next = NULL;
        freeSymbol(toRemove);
    }
}

/*	Initialize symbol table with NULLs  */
void symTableInit() {
    int i;
    
    for(i=0; i < HASHSIZE; i++)
            symTable[i] = NULL;
}

/*	Release symbol table */
void symTableRelease() {
    int i;
    
    for(i=0; i < HASHSIZE; i++) {
        if(symTable[i] != NULL) {
            freeSymbol(symTable[i]);
            symTable[i] = NULL;
        }
    }
}

/* freeSymbol:	free symbol and all symbols attached to it. */
void freeSymbol(symbol *s) {
    if (s->next != NULL) {		/* If chained to more symbols recursively free them */
        freeSymbol(s->next);
    }
    free(s);
}

/* symbolAddressAdd: adds x to non-external symbols' address */
void symbolsAddressAdd(int x) {
    int i;
    
    for(i=0; i < HASHSIZE; i++) {
        if(symTable[i] != NULL)
            addressUpdate(symTable[i], x);
    }
}

/* Updates symbol s address with addition of x (IC) if internal variable.
 * Add MEMORY_START_ADDRESS to all non-external symbols.
 * and continue to chained symbols */
void addressUpdate(symbol *s, int x) {
    if (s->next != NULL)		/* If chained to more symbols recursively update them as well */
        addressUpdate(s->next, x);
    
    /* Adds to internals variables */
    if (s->type == DATA)
        s->address += (x+MEMORY_START_ADDRESS);
    else if (s->type == COMMAND)
    	s->address += MEMORY_START_ADDRESS;
}

/* Print symbols table. Used for debugging */
void printSymTable() {
	int i;

	printf("Symbols Table:\n\n ");
	for (i=0; i<HASHSIZE; i++) {
		if (symTable[i])
			printSym(symTable[i]);
	}
}

/* Print symbol sym. Utility function for printSyTable */
void printSym(symbol *sym) {
	char code[3];

	intToBase32STR(sym->address, code);
	printf("%s:\tAddress: %d, Coding: %s\n",sym->name, sym->address, code);
	if (sym->type == COMMAND)
		intToBase32STR((((sym->address)<<2)), code);
	else if (sym->address == EXTERNAL_ADDRESS)
		intToBase32STR((((sym->address)<<2)|1), code);
	else
		intToBase32STR((((sym->address)<<2)|2), code);
	printf("\tCoding with coding type: %s\n\n", code);

	if(sym->next)
		printSym(sym->next);
}
