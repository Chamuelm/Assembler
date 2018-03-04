/* 
 * File:    firstPass.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declerations of firstPass.c
 * 
 */

#ifndef FIRSTPASS_H
#define FIRSTPASS_H

int firstPass();
void procCommand(line *l, char *word);
int addData(int val);
void addInstruction(operatorNode c, operand operand1, operand operand2, int lineNumber);
void procData(line *l);
void procString(line *l);
void procStruct(line *l);
void procExtern(line *l);

#endif /* FIRSTPASS_H */

