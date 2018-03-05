/* 
 * File:    symbol.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declerations of symbol.c
 * 
 */

#ifndef SYMBOL_H
#define SYMBOL_H

/******************** Extern symbol table decleration ***********************/
#ifndef SYMBOL_C
#define SYMBOL_C 
extern symbol *symTable[HASHSIZE];
#endif

/************************* Constants definition ****************************/
#define HASHSIZE 101

/*********************** Structs definition **************************/
typedef struct {
	char name[MAX_LABEL_LEN];	/* Symbol name */
	int address;			/* Address in memory */
	enum lineTypes type;            /* Symbol type (COMMAND/DATA/EXTERN) */
        unsigned int isEntry : 1;       /* flag if is entry */
	symbol *next;                   /* Next symbol in chain */
} symbol;

/********************* 	Functions Declerations 	************************/
unsigned int hash(char *s);
symbol *symLookup(char *s);
symbol *addSymbol(char *newName, int newAddress, enum lineTypes newType);
char *strdup(char *s);
void symTableInit();
void freeSymbol(symbol *s);
void symbolsAddressAdd(int x);
void addressUpdate(symbol *s, int x);

#endif