/* 
 * File:    symbol.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declerations of output.c
 * 
 */
#ifndef OUTPUT_H
#define OUTPUT_H

void createOutputFiles();
void createObjectsFile();
void intToBase32STR(int num, char *dest);
void fAddLine(FILE *fileP, char *str1, char *str2);
void fAddInst(FILE fileP, instruction x);
void fAddInst0op(FILE fileP, instruction x);
void fAddInst1op(FILE fileP, instruction x);
void fAddInst2op(FILE fileP, instruction x);
void fAddCommandWord(FILE fileP, instruction x);
void fAddImmediateWord(FILE *fileP, char *data, int address);
void fAddRegisterWord(FILE *fileP, char *srcData, char *destData, int address);
void fAddDirectWord(FILE *fileP, char *data, int address);
int getAddressingTypeCode(enum addressingType type);
void createExtEntFiles();
void fAddExtEnt(symbol *sym, FILE *ext, FILE *ent);
FILE *extFileOpen();
FILE *entFileOpen();



#endif /* OUTPUT_H */

