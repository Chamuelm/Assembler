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
			lerror(ERR_PARAM_EXTRA, l->lineNum);
			return;
		}
		else
			addInstruction(c, NULL, NULL);

		break;
	case 1:
		/* Check comma before parameters */
		if(checkComma(l))
		{
			lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
			l->i++;
		}

		if()

	}

}


/* calcInstructions:	Calculate number of instructions  
 * based on operators left in line
 */ 
int calcInstructions(line *l)
{
	enum addressingType op1Type, op2Type;
	int i = 1;

	getOperandsType(l, &op1Type, &op2Type);
	
	/* 
	 * Increment i by type of addresing type:
	 *	All but struct required 1 additional word
	 *	Struct required 2 additional words.
	 *	If both are REGISTER they can share the same word  
	  */
	switch(op1Type)
	{
	case EMPTY:
		break;
	case REGISTER:
	case IMMEDIATE:
	case DIRECT:
		i++;
		break;
	case STRUCT_ADD:
		i += 2;
	}
	
	switch(op2Type)
	{
	case EMPTY:
		break;
	case REGISTER:
		if (op1Tryp != REGISTER)
			i++;											/* If op1Type is REGISTER it use */
		break;											/* the same additional word */
	case IMMEDIATE:
	case DIRECT:
		i++;
		break;
	case STRUCT_ADD:
		i += 2;
	}
	
	return i;
}

/* Puts operands types in received pointers */ 
void getOperands(line *l, *op1Type, *op2Type)
{
	char *p;
	
	skipWhite(l);								/* Skip white spaces */
	p = strtok((l->data)+(l->i), ",");
	if (!p)											/* No operands received */
	{
		*op1Type = *op2Type = EMPTY;
		return;
	}
	else if (*p == '#')
		*op1Type = IMMEDIATE;
	else if (isRegisterName(p))
		*op1Type = REGISTER;
	else if (isStructName(p))
		*op1Type = STRUCT_ADD;
	else
		*op1Type = DIRECT;
	
	p = strtok(NULL, ",");
	if (!p)											/* No operands received */
		*op2Type = EMPTY;
	else if (*p == '#')
		*op2Type = IMMEDIATE;
	else if (getRegisterNum(p) != FALSE_RETURN)
		*op2Type = REGISTER;
	else if (isStructName(p))
		*op2Type = STRUCT_ADD;
	else
		*op2Type = DIRECT;
}

/* 
 * isStructName: Check if s is a struct template
 * ie. name.1 or name.2
 */ 
int isStructName(char *s)
{
	int n, i;
	
	n = strlen(s);
	i = 1;
	/* 
	 * Struct name must be at lease one char so
	 * start look for end of struct name in index 1
	 */ 
	
	while (i < n)
	{
		if (s[i] == '.')		/* Looks for '.' after struct name */
		{
			if(i+1<n && (s[i+1] == '1' || s[i+1] == '2'))
				return TRUE_RETURN;
			else
				return FALSE_RETURN;
		}
	}
	return FALSE_RETURN;
}

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

/* checkData:	Check if received number supported by 10-bit computer */
int checkData(int val)
{
	return (val<=MAX_NUMBER && val >= MIN_MUNBER);
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
}

/* isValidDataNum: Check if string s is not valid number
 * Prints to stderr if is not valid */
int isValidDataNum(char *s, int line)
{
	int i, count;
	
	count = i = 0;
	
	sSkipWhite(s, &i); /* Skips white spaces */
	
	/* Skip sign */
	if (s[i] == '+' || s[i] == '-')
	{
		i++;
		/* Check if received lonely sign */
		if (s[i] == '\0')
		{
			lerror("Received sign without number", line);
			return FALSE_RETURN;
		}
	}
	else
	{
		/* Check if received empty string 
		 * Empty string means invalid comma */
		if (s[i] == '\0')
		{
			lerror("Invalid comma: Missing parameter between commas or extra comma at the end", line);
			return FALSE_RETURN;
		}
	}
		
	/* Check if the rest are digits */
	while (s[i] != '\0')
	{
		if (isdigit[s[i]])	/* Skip if digit */
		{
			count++;
			i++;
		}
		else if (s[i] == ' ' || s[i] == '\t')		/* If white space check if finish or have another parameter */ 
		{
			if (count == 0)		/* No numbers received so lonely sign */
				lerror("Received sign without number", line);
			sSkipWhite(s, &i);
			if (s[i] != '\0')			/* If there is another parameter warn for missing comma and check the next one */
			{
				lerror("Missing comma between parameters", line);
				if (isValidDataNum(s+i))
					addDataNumber(atoi(s+i));
			}
		}
		else
		{
			lerror("Invalid character", line);
			return FALSE_RETURN;
		}
	}

	if(checkData(atoi(s)))		/* Check if number is in hardware's range */
		return TRUE_RETURN;
	else
	{
		lerror("Data parameter is too big.", line);
		return FALSE_RETURN;
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
