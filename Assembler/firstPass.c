#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* firstPass: process first pass on current active file */
int firstPass()
{
	int isValidSymbol;							/* Symbol flag */
	int lineC;											/* Line counter */
	/* Temporary variables */
	enum errorsShort err;
	enum lineTypes lType;
	char *word;
	symbol *sym;
	line *currLine;

	lineC = 0;											/* Line counter initialize */

	/* Processing each line */
	while (fgets(currLine->data, MAX_LINE_LEN, fp)) {
		lineC++;
		currLine = lineInit(lineC);

		/******************** Symbol check processing ********************/
		word = getWord(currLine);
		if (!word) /* Empty line */
			continue;	/* Skip to next line */

		/* if it is a symbol declaration check symbol
		 * validity and get next word in line
		 */
		if (isSymbol(word))
		{
			removeColon(word);
			if ((err = checkSymbol(word)) == NON_ERROR)
			{
				isValidSymbol = 1;
				sym = addSymbol(word, EMPTY_ADDRESS, UNKNOWN);
			}
			else
			{
				isValidSymbol = 0;
				lerror(err,lineC);
			}

			free(word);		/* Release dynamically allocated memory */
			/* Receive next word in line to process*/
			word = getWord(currLine);
			if(!word)	/* No more words in line = label in empty line */
			{
				lwarning(ERR_LABEL_EMPTY_LINE, lineC);
				continue;		/* Continue to next line */
			}
		}

		/******************** Actual line processing ********************/
		lType = getLineType(word);

		if(lType == UNKNOWN)		/* If unknown command, continue to next line */
		{
			lerror(ERR_UNKNOWN_CMD, lineC);
			continue;
		}

		/***************** Symbol data update if needed *****************/
		if(isValidSymbol)
		{
			switch(lType)
			{
			case COMMAND:
				sym->type = COMMAND;
				sym->address = IC;
				break;
			case DATA: case STRING: case STRUCT:
				sym->type = DATA;
				sym->address = DC;
				break;
			case entry:
				lwarning(ERR_LABEL_ENTRY, lineC);
				removeSymbol(sym);
				break;
			}
		}

		/******************* Continue line processing *******************/
		switch (lType) {
		case COMMAND:
			procCommand(currLine, word);
			break;
		case DATA:
			procData(currLine);
			break;
		case STRING:
			procString(currLine);
			break;
		case STRUCT:
			procStruct(currLine);
			break;
		case ENTRY:		/* Skip if entry */
			break;
		case EXTERN:
			procExtern(currLine);
			break;
		}

		/* Memory release */
		free(word);
		free(currLine);
	}
}


void procCommand(line *l, char *word)
{
	command *c;
	operand *op1, *op2;
	c = getCommand(word);

	switch(c->operatorsNum)
	{
	case 0:
		/* Check number of parameters */
		if(!isEOL(l))
		{
			lerror(ERR_MORE_PARM0, l->lineNum);
			return;
		}
		else
			addInstruction(c, NULL, NULL, l->lineNum);

		break;


	case 1:	/* Get one operands */
		/* Check comma before parameters */
		if(checkComma(l))
		{
			lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
			l->i++;
		}

		op1 = getOperand(l);
		if(!op1)
			lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
		else
		{	/* Other line validity checks */
			if (checkComma(l))
			{
				if (getOperand(l))
					lerror(ERR_MORE_PARAM1, l->lineNum); /* Too many parameters */
				else
					lerror(ERR_COMMA_EXTRA, l->lineNum); /* Extra comma after parameter */
			}
			else
			{
				if (isEOL(l))
					addInstruction(c, op1, NULL, l->lineNum);
				else
					lerror(ERR_CMD_EXTRA_TEXT, l->lineNum);
			}

		}
		break;


	case 2:	/* Get 2 operands */
		/* Check comma before parameters */
		if(checkComma(l))
		{
			lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
			l->i++;
		}
		op1 = getOperand(l);
		if(!op1)
			lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
		else
		{
			if(!getComma(l))	/* look for comma between parameters */
			{
				op2 = getOperand(l);
				if(!op2)	/* If no comma and not operand */
					lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
				else
				{
					lerror(ERR_COMMA_EXPECTED, l->lineNum);
					addInstruction(c, op1, op2, l->lineNum);
				}
			}
			else /* received comma */
			{
				op2 = getOperand(l);
				if(!op2)
					lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
				else	/* No errors */
					addInstruction(c, op1, op2, l->lineNum);
			}
		}
		break;


	}	/* End switch */
}	/* End function */





/*
 * addData:	add given number and put in data array
 * Given number is already in 2's complement so no need to convert.
 * */
int addData(int val)
{
	if (DC < MAX_INSTRUCTIONS)
	{
		if (val > 0)	/* Turn sign bit off */
			dataArr[DC++] = val&(~SIGN_BIT_MASK);
		else					/* Turn sign bit on */
			dataArr[DC++] = val|SIGN_BIT_MASK;
	}
	else
	{
		fprintf(stderr, "Data array is too small. Exiting...\n");
		exit(EXIT_FAILURE);
	}
}

void addInstruction(command c, operand operand1, operand operand2, int lineNumber)
{
	instArr[instIndex].cmd = c;
	insrArr[instIndex].op1 = operand1;
	insrArr[instIndex].op2 = operand2;
	insrArr[instIndex].lineNum = lineNumber;

	IC += caclinstructions(instArr[instIndex]);	/* Instructions counter increment */
	instIndex++;								/* Instructions array index increment */
}




/* procData:	Stores data in data table, increment DC, and check for errors */ 
void procData(line *l)
{
	char *p;						/* Temp string pointer */

	skipWhite(l);

	/* Check for comma before data */
	if (l->data[l->i] == ',')
	{
		lerror("Invalid comma before data", l->lineNum);
		l->i++;
	}

	/* Receive tokens separated by commas */
	while (p = strtok((l->data)+(l->i), ","))
	{
		if(isValidDataNum(p, l->lineNum))
			addData(atoi(p));
	}
}

/* procString:	Stores string in data table, increment DC, and check for errors */
void procString(line *l)
{
	char c;
	skipWhite(l);

	c = l->data[l->i++];
	if(c == "\"")	/* Start of string */
	{
		/* Add chars to data table until end of string */
		c = l->data[l->i++];
		while (c != '\n')
		{
			if (c == "\"")	/* = end of string */
			{
				addData('\0');
				break;
			}
			else
				addData(c);
		}

		if (c == '\n')	/* Error if not received closing quotation mark */
		{
			lerror("Expected \" (Quotation marks) in end of line", l->lineNum);
		}
	}
	else	/* Error detection */
	{
		if (c == '\n')
			lerror("Missing string parameter", l->lineNum);
		else
			lerror("Invalid string parameter, String must be in quotation marks", l->lineNum);
	}
}

/* procStruct:	Stores struct in data table, increment DC, and check for errors */
void procStruct(line *l)
{
	char *temp;

	if (checkComma(l))		/* Received comma before parameter */
	{
		lerror("Extra comma before parameter", l->lineNum);
		l->i++;
	}

	temp = getParameter(l);
	if (!temp)
	{
		lerror("Missing parameter", l->lineNum);
		return;
	}

	if (isValidNum(temp, l->lineNum))
	{
		addData(atoi(temp));	/* Add struct's number to data array */
		free(temp);
	}

	/* Look for comma between parameters */
	if (checkComma(l))
		l->i++;
	else
		lerror("Expected comma", l->lineNum);

	/* Process struct's string */
	procString(l);
}

/* procExtern:	Stores externs in data table, increment DC, and check for errors */
void procExtern(line *l)
{
	char *c;

	if (checkComma(l))
	{
		lerror("Invalid comma before parameter", l->lineNum);
		l->i++;
	}
	while (c = getParameter(l))
	{
		if
	}
}
