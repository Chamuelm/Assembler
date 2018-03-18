/* 
 * File:    symbol.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declarations of symbol.c
 * 
 */
#ifndef ASSEMBLER_H
#include "assembler.h"
#endif

#ifndef SYMBOL_H
#define SYMBOL_H

/************************** Struct definition *****************************/
typedef struct symbol_s symbol;
struct symbol_s {
    char name[MAX_LABEL_LEN];	/* Symbol name */
    int address;			/* Address in memory */
    enum lineTypes type;            /* Symbol type (COMMAND/DATA/EXTERN) */
    unsigned int isEntry : 1;       /* flag if is entry */
    symbol *next;                   /* Next symbol in chain */
};

/******************** Extern symbol table declaration ***********************/
#ifndef SYMBOL_C
#define SYMBOL_C 
extern symbol *symTable[HASHSIZE];
#endif

/********************* External Functions Declarations ************************/

symbol *symLookup(char *s);
symbol *addSymbol(char *newName, int newAddress, enum lineTypes newType);
void symTableInit();
void symTableRelease();
void symbolsAddressAdd(int x);
void removeSymbol(symbol *toRemove);
void printSymTable();

#endif
