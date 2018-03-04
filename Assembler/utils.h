/* 
 * File:    utils.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declerations of utils.c
 * 
 */

#ifndef UTILS_H
#define UTILS_H

void removeColon(char *symbolName);
enum errorsShort checkSymbol(char *symbol);
enum lineTypes getLineType(char *cmd);
operatorNode *getCommand(char *cmd);
int isSymbol(char *s);
int isKeyword(char *s);
int isRegister(char *s);
enum errorsShort isValidNum(char *s);
int isStruct(char *s);
enum errorsShort checkStructName(char *s);
int calcInstructions(instruction *inst);

#endif /* UTILS_H */

