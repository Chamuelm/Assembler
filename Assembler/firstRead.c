#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* firstPass: proccess first pass on current active file */
int firstPass() {
	char word[MAX_LABEL_LEN];				/* Temp word string */
	char symbolName[MAX_LABEL_LEN];	/* Holds current symbol name */
	char line[MAX_LINE_LEN];				/* Line buffer */
	int lineIndex;									/* Line buffer index */	
	int lineC = 0;									/* Line counter */
	enum lineTypes lineType;				/* Line type */
	int symbolAddress;							/* Symbol address */
	int flags, i;

	/* Proccessing each line */
	while (line = fgets(line, MAX_LINE_LEN, fp)) {
		lineC++;
		lineIndex = symbolAddress = flags = 0;	/* Vars init */

		skipWhite(line, &lineIndex);	/* Skip white spaces before line */
		if (line[0] == '\0' || line[0] == '\n') /* If empty line */
			continue;

		i = getWord(line, &lineIndex, word ,&flags); /* Reads first word in line */
		 
		/* Input checks */
		if (flags & WORD_TOO_LONG)	/* Word too long */
		{
			lerror("Word too long", lineC);
			continue;
		}
		
		if (word[i-1] == ':')		/* Received label */
		{
			if (i == 1)		/* Error - No label name received */
			{
				lerror("Empty label", lineC);
				continue;
			}
			strcpy(symbolName, word);

			/* Get next word in line */
			if (!getWord(line, &lineIndex, word ,&flags))
			{
				lerror("Empty line after label", lineC);
				continue;
			}
		}

		lineType = getLineType(word);
		switch (lineType) {
		case COMMAND:
			if (isSymbol)
				symbolAddress = IC;
			procCommand(word, line, lineIndex, lineC);
			break;
		case DATA:
			if (isSymbol)
				symbolAddress = DC;
			procData(word, line);
			break;
		case STRING:
			if (isSymbol)
				symbolAddress = DC;
			procString(word, line);
			break;
		case STRUCT:
			if (isSymbol)
				symbolAddress = DC;
			procStruct(word, line);
			break;
		case ENTRY:
			procEntry(word, line);
			break;
		case EXTERN:
			procExtern(word, line);
			break;
		}

		if (checkLabel(labelName, lineC)) /* Validity check - errors inside the function */
			addSymbol(symbolName, symbolAddress, lineType);
	}
}
}

/* procCommand: proccess command line */
void procCommand(char *commName, char *line, &lineIndex, lineNumber)
{
	command c;
	operand o1, o2;

	c = checkCommand(commName);
	if (c->operatorsNum == 2)
	{
		o1 = getOperand(line, lineIndex);
		o2 = getOperand(line, lineIndex);
	}
}

/* getOperand: received line and index in line and return the next operand */
operand *getOperand(char *line, int *lineIndex)
{
	char word[MAX]
}
