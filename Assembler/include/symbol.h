/* 
 * File:    symbol.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declerations of symbol.c
 * 
 */
#ifndef ASSEMBLER_H
#include "assembler.h"
#endif

#ifndef SYMBOL_H
#define SYMBOL_H

/************************* Constants definition ****************************/
#define HASHSIZE 101

/************************** Struct definition *****************************/
typedef struct symbol_s symbol;
struct symbol_s {
    char name[MAX_LABEL_LEN];	/* Symbol name */
    int address;			/* Address in memory */
    enum lineTypes type;            /* Symbol type (COMMAND/DATA/EXTERN) */
    unsigned int isEntry : 1;       /* flag if is entry */
    symbol *next;                   /* Next symbol in chain */
};

/******************** Extern symbol table decleration ***********************/
#ifndef SYMBOL_C
#define SYMBOL_C 
extern symbol *symTable[HASHSIZE];
#endif

/********************* External Functions Declerations ************************/

symbol *symLookup(char *s);
symbol *addSymbol(char *newName, int newAddress, enum lineTypes newType);
void symTableInit();
void symTableRelease();
void symbolsAddressAdd(int x);
void removeSymbol(symbol *toRemove);

#endif