/* 
 * File:    utils.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains external function declerations of utils.c
 * 
 */

#ifndef UTILS_H
#define UTILS_H

#include "assembler.h"

error checkSymbol(char *symbol);
operatorNode *getCommand(char *cmd);
int isKeyword(char *s);
int isSymbol(char *s);
int isRegister(char *s);
int isStruct(char *s);
error isValidNum(char *s);
error checkStructName(char *s);
error checkEntry(char *s);
char *strdcat(char *dest, const char *addition);
char *strdup(char *s);
unsigned int hash(char *s);
void seekEOL(FILE *fileP);

#endif /* UTILS_H */

