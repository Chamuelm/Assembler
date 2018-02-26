#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* firstPass: proccess first pass on current active file */
int firstPass() {
	int wordCount;
	line currLine;
	int lineC = 0;									/* Line counter */
	char word[MAX_LINE_LEN];				/* Temp word string *
	/*
	char line[MAX_LINE_LEN];				/* Line buffer *
	int lineIndex;									/* Line buffer index *
	enum lineTypes lineType;				/* Line type *
	int symbolAddress;							/* Symbol address *
	int flags, i;
	*/

	/* Processing each line */
	while (fgets(currLine.line, MAX_LINE_LEN, fp)) {
		lineC++;
		lineInit(&currLine, lineC);

		/* Proceed to first non-blank char and check if empty line */
		if (checkEmpty(&currLine))
			continue;

		wordCount = (getWord(&currLine, word); /* Reads first word in line */
		if (currLine.flags&LINE_TOO_LONG)
			lerror("Line too long.", currLine.lineNum);
		if(isLabel(&currLine, word, &wordCount))
		{												/* If first word is label check label for errors
														 * and get line actual first word */
			checkLabel(&currLine, word, wordCount);

			/* Check if there is more words in line */
			if (isEOL(&currLine))
			{
				lerror("Label is not attached to instruction or data", currLine.lineNum);
				currLine.flags |= INVALID_SYM;
				continue;
			}
			getWord(&currLine, word);

			/* Remove symbol flag if invalid */
			if (currLine.flags&INVALID_SYM)
				currLine.flags |= VALID_SYM;
		}
		 
		lineType = getLineType(word);
		switch (lineType) {
		case COMMAND:
			if (isSymbol(currLine))
				currLine.symbolAddress = IC;
			procCommand(&currLine, word);
			break;
		case DATA:
			if (isSymbol(currLine))
				currLine.symbolAddress = DC;
			procData(&currLine, word);
			break;
		case STRING:
			if (isSymbol(currLine))
				currLine.symbolAddress = DC;
			procString(&currLine, word);
			break;
		case STRUCT:
			if (isSymbol(currLine))
				currLine.symbolAddress = DC;
			procStruct(&currLine, word);
			break;
		case ENTRY:
			procEntry(&currLine, word);
			break;
		case EXTERN:
			procExtern(&currLine, word);
			break;
		}

		if (checkLabel(symbolName, lineC)) /* Validity check - errors inside the function */
			addSymbol(symbolName, symbolAddress, lineType);
	}
}
}

/* procCommand: process command line */
void procCommand(line *l, char *cmdName)
{
	command *c;
	operand o1, o2;

	c = checkCommand(cmdName);
	if (c == NULL)
	{
		lerror("Unknown command", l->lineNum);
		return;
	}

	if (!getOperands(l, &o1, &o2))
		return;				/* Finish process if operands are not valid */

	if (c->operatorsNum == 2)
	{


	}
}

/* getOperand: received line and index in line and return the next operand */
int getOperands(line *l, operand *o1, operand *o2)
{
	char op1[MAX_LINE_LEN], op2[MAX_LINE_LEN], *c;
	int isValid = 1;
	op1[0] = op2[0] = '\0';

	skipWhite(l);
	if (l->data[l->i] == ',')
	{
		lerror("Invalid comma before operand", l->lineNum);
		isValid = 0;
		l->i++;
	}

	c = strtok((l->data)+(l->i), ",");		/* Divide by ',' to tokens */
	if (!c)		/* No operands */
	{
		o1->type = o2->type = EMPTY;
		o1->data[0] = o2->data[0] = '\0';
	}
	else
	{
		strcpy(o1->data, c);
		o1->type = getOperandType(c);

		c = strtok(NULL, ",");
		if(!c)
		{
			o2->type = EMPTY;
			o2->data[0] = '\0';
		}
		else
		{
			strcpy(o2->data, c);
			o2->type = getOperandType(c);

			if (strtok(NULL, ","))
			{
				lerror("Too many operators", l->lineNum);
				isValid = 0;
			}
		}
	}

	return isValid;
}
