/* 
 * File:    line.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains function declarations of line.c
 * 
 */

#ifndef LINE_H
#define LINE_H

#include "assembler.h"

/************************** Struct definition *****************************/

/* Holds line information to pass between functions */
typedef struct {
	char data[MAX_LINE_LEN];	/* Contains actual line */
	int lineNum;							/* Line number in file */
	int i;										/* Line index */
} line;

line *lineInit(int lineNumber);
char *getWord(line *l);
char *getParameter(line *l);
operand *getOperand(line *l, int reportError);
void skipWhite(line *l);
int isEOL(line *l);
int checkComma(line *l);
enum lineTypes getLineType(char *cmd);
error checkEOL(line *l);
int isCompleteLine(line *l);

#endif /* LINE_H */

