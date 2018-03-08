/* 
 * File:    symbol.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains symbols table decleration and functions related.
 * Symbols table is a hash table with the size HASHSIZE which is
 * declared in symbol.h file
 * 
 */
#define SYMBOL_C
#include "./include/assembler.h"
#include <stdlib.h>
#include <stdio.h>

symbol *symTable[HASHSIZE];		/* Symbols table */

/* hash:	form hash value for string s */
unsigned int hash(char *s) {
    unsigned int hashval;
    
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31*hashval;
    return hashval % HASHSIZE;
}

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
        np = (symbol *) malloc(sizeof(*np));
        if(np == NULL || (np->name = strdup(newName)) == NULL)	/* Successful alloc? */
            exitMemory();	/* Exit if memory allocation failed */
        
        /* Put in symbol table by hash */
        hashval = hash(newName);
        np->next = symTable[hashval];
        np->address = newAddress;
        np->type = newType;
        np->isEntry = FALSE;
        symTable[hashval] = np;
        
        /* Address update by first memory address if symbol is not external */
        if (newType != EXTERN)
            np->address += MEMORY_START_ADDRESS;
    } else	/* Symbol already exist! */
        return NULL;
    return np; /* symboll added fine */
}

/* strdup: Copy string s to returned new allocated space */
char *strdup(char *s) {
    char *p;
    p = (char *) malloc(strlen(s) + 1);
    if(p != NULL)
        strcpy(p, s);
    return p;
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

/*	Initialize symbol table */
void symTableInit() {
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
    if (s->next != NULL)		/* If chained to more symbols recursivly free them */
        freeSymbol(s->next);
    free(s->name);					/* Free name pointer */
    free(s);								/* Free symbol pointer */
}

/* symbolAddressAdd: adds x to non-external symbols' address */
void symbolsAddressAdd(int x) {
    int i;
    
    for(i=0; i < HASHSIZE; i++) {
        if(symTable[i] != NULL)
            addressUpdate(symTable[i], x);
    }
}

/* Updates symbol s address with addition of x if internal variable 
 * and continue to chained symbols */
void addressUpdate(symbol *s, int x) {
    if (s->next != NULL)		/* If chained to more symbols recursivly update them as well */
        addressUpdate(s->next, x);
    
    /* Adds to internals variables */
    if ((s->type != EXTERN) && (s->type != COMMAND))
        s->address += x;
}