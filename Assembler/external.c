/* 
 * File:    external.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains external table declaration and functions related.
 * Externals table is a hash table with the size HASHSIZE which is
 * declared in assembler.h file
 * 
 */
#define EXTERNAL_C
#include "include/assembler.h"
#include <stdlib.h>

/***************** Externals table declaration *****************/
externNode *extTable[HASHSIZE];

/***************** Internal functions declaration *****************/
void freeExternal(externNode *e);

/* addExternal:	add new external to externals table */
externNode *addExternal(symbol *symToAdd, int addressToAdd) {
    externNode *np;
    unsigned hashval;
    
    np = (externNode *)malloc(sizeof(externNode));
    if(np == NULL)	/* Successful alloc? */
        exitMemory();	/* Exit if memory allocation failed */
    
    /* Put in external table by hash */
    hashval = hash(symToAdd->name);
    np->next = extTable[hashval];
    np->address = addressToAdd;
    np->sym = symToAdd;
    extTable[hashval] = np;
    
    return np; /* External added fine */
}

/*	Initialize external table with NULLs  */
void extTableInit() {
    int i;
    
    for(i=0; i < HASHSIZE; i++)
            extTable[i] = NULL;
}

/*	Release memory of externals in externals table */
void extTableRelease() {
    int i;
    
    for(i=0; i < HASHSIZE; i++) {
        if(extTable[i] != NULL) {
            freeExternal(extTable[i]); /* Recursive function */
            extTable[i] = NULL;
        }
    }
}

/* freeExternal:	free external and all externals chained to it.
 * Utility function for extTableRelease */
void freeExternal(externNode *e) {
    if (e->next != NULL) {		/* If chained to more external recursively free them */
        freeExternal(e->next);
    }
    free(e);
}
