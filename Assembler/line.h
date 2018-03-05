/* 
 * File:    line.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declerations of line.c
 * 
 */

#ifndef LINE_H
#define LINE_H

line *lineInit(line *oldLine, int lineNumber);
char *getWord(line *l);
char *getParameter(line *l);
operand *getOperand(line *l);
void skipWhite(line *l);
int checkEmpty(line *l);
int isEOL(line *l);
int checkComma(line *l);
enum errorsShort checkEntry(char *s);

#endif /* LINE_H */

