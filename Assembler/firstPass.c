/* 
 * File:    firstPass.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains functions related to first pass of the assembler on input file
 * 
 */
#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* firstPass: process first pass on current active file */
int firstPass()
{
	int isValidSymbol;			/* Symbol flag */
	int lineC;				/* Line counter */
	
        /* Temporary variables */
	enum errorsShort err;
	enum lineTypes lType;
	char *word;
	symbol *sym;
	line *currLine;

        /* Line counter initialize */
	lineC = 0;

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
			case ENTRY:
                             /* Print warning if label attached to entry 
                              and remove from symbol table */
                            lwarning(ERR_LABEL_ENTRY, lineC);
                            removeSymbol(sym);
                            break;
                        case EXTERN:
                            /* Print error if label attached to entry 
                              and remove from symbol table */
                            lerror(ERR_LABEL_EXTERN, lineC);
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
        
        /* Update symbols address by IC to all external symbols address */
        symbolsAddressAdd(DC);
}


void procCommand(line *l, char *word)
{
	operatorNode *c;
	operand *op1, *op2;
	c = getCommand(word);

        /* Continue process by number of parameters needed */
	switch(c->operatorsNum) 
	{
	case 0:
		/* Check number of parameters */
		if(!isEOL(l))
		{
			lerror(ERR_MORE_PARAM0, l->lineNum);
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
			if(!checkComma(l))	/* look for comma between parameters */
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
	if (DC < MAX_INSTRUCTIONS) /* Check for space in data array */
	{
		if (val > 0)	/* Turn sign bit off */
			dataArr[DC++] = val&(~SIGN_BIT_MASK);
		else		/* Turn sign bit on */
			dataArr[DC++] = val|SIGN_BIT_MASK;
	}
	else
	{
		fprintf(stderr, "Data array is too small. Exiting...\n");
		exit(EXIT_FAILURE);
	}
}

/* addinstruction: Add instruction node to instructions array
 * and calculate number of actual assembly instructions and adds to
 * instructions counter IC
 */
void addInstruction(operatorNode c, operand operand1, operand operand2, int lineNumber)
{
	instArr[instIndex].cmd = c;
	instArr[instIndex].op1 = operand1;
	instArr[instIndex].op2 = operand2;
	instArr[instIndex].lineNum = lineNumber;

	IC += calcInstructions(instArr[instIndex]);	/* Instructions counter increment */
	instIndex++;					/* Instructions array index increment */
}


/* procData:	Stores data in data table (number), 
 * increment DC, and check for errors */ 
void procData(line *l)
{
	char *p;			/* Temp string pointer */
        enum errorsShort err;           /* error type holder */

	skipWhite(l);

	/* Check for comma before data */
	if (l->data[l->i] == ',')
	{
		lerror("Invalid comma before data", l->lineNum);
		l->i++;
	}

	/* Receive tokens separated by commas */
        p = strtok((l->data)+(l->i), ",");
	while (p)
	{
            err = isValidNum(p);
            if (err == NON_ERROR)
                addData(atoi(p));
            else
                lerror(err, l->lineNum);
            
            /* Get next token */
            p = strtok(NULL, ",");
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
			lerror(ERR_STR_CLOSE, l->lineNum);
		}
	}
	else	/* Error detection */
	{
		if (c == '\n')  /* No string parameter */
			lerror(ERR_STR_MISS, l->lineNum);
		else            /* String is not in qoutation marks */
			lerror(ERR_STR_QMARK, l->lineNum);
	}
}

/* procStruct:	Stores struct in data table, increment DC, and check for errors */
void procStruct(line *l)
{
	char *temp;

	if (checkComma(l))		/* Received comma before parameter */
	{
		lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
		l->i++;
	}

	temp = getParameter(l);
	if (!temp)
	{
		lerror(ERR_MISS_PARAM, l->lineNum);
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
		lerror(ERR_COMMA_EXPECTED, l->lineNum);

	/* Process struct's string */
	procString(l);
}

/* procExtern:	Stores externs in data table and check for errors */
void procExtern(line *l)
{
	char *str;
        enum errorsShort err;

	if (checkComma(l))
	{
		lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
		l->i++;
	}
	while (str = getParameter(l))
	{
            err = checkSymbol(str);
            if (err == NON_ERROR)
                addSymbol(str, EXTERN_DEFAULT_ADD, EXTERN);
            else
                lerror(err, l->lineNum);
            
            free(str);
	}
}
