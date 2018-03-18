/* 
 * File:    external.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declerations of external.c
 * And externals data table
 * 
 */
#ifndef ASSEMBLER_H
#include "assembler.h"
#endif

#ifndef EXTERNAL_H
#define EXTERNAL_H


/************************** Struct definition *****************************/
/* struct external_s:   Contain information about external symbol and in
 *                               what address it is used 
 */
typedef struct externNode_s externNode;
struct externNode_s {
    symbol *sym;				/* Pointer to external symbol in symbol table */
    int address;				/* Address it is used in current as file */
    externNode *next;		/* Pointer to next external in chain */
};



/******************** Extern symbol table declaration ***********************/
#ifndef EXTERNAL_C
#define EXTERNAL_C 
extern struct externNode_s *extTable[HASHSIZE];
#endif

/********************* External Functions Declarations ************************/
externNode *addExternal(symbol *symToAdd, int addressToAdd);
void extTableInit();
void extTableRelease();


#endif /* EXTERNAL_H */

