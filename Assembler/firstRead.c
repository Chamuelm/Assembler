#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* firstPass: proccess first pass on current active file */
int firstPass() {
	int wordCount;
	line currLine;
	int lineC = 0;									/* Line counter */
	char word[MAX_LINE_LEN];				/* Temp word string */
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
			if (isSymbol(currLine))	/* If is symbol, saves address information */
				currLine.symbolAddress = IC;
			/* Determine number of instructions and adds to IC */
			IC += calcInstructions(&currLine, word);
			break;
		case DATA:
			if (isSymbol(currLine))	/* If is symbol, saves address information */
				currLine.symbolAddress = DC;
			procData(&currLine, word);
			break;
		case STRING:
			if (isSymbol(currLine))	/* If is symbol, saves address information */
				currLine.symbolAddress = DC;
			procString(&currLine, word);
			break;
		case STRUCT:
			if (isSymbol(currLine))	/* If is symbol, saves address information */
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

/* Create This Function!!!!!! */
getLineType(word)


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
void getOperandsType(line *l, *op1Type, *op2Type)
{
	char *p
	
	skipWhite(l);								/* Skip white spaces */
	p = strtok((l->data)+(l->i), ",");
	if (!p)											/* No operands received */
	{
		*op1Type = *op2Type = EMPTY;
		return;
	}
	else if (*p = '#')
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
	else if (*p = '#')
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

/* procData:	Stores data in data table, increment DC, and check for errors */ 
void procData(l)
{
	char *p;						/* Temp string pointer */
	
	skipWhite(l);
	
	/* Check for comma before data */
	if (l->data[l->i] == ',')
	{
		lerror("Invalid comma before data", l->lineNum);
		l->i++;
	}
	
	/* Receive tokens sepereted by commas */
	while (p = strtok((l->data)+(i), ","))
	{
		if(isValidDataNum(p, l->lineNum))
			addDataNumber(atoi(p));
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
			if (s[i] != '\0')			/* If there is anothe parameter warn for missing comma and check the next one */
			{
				lerror("Missing comma between parameters", line);
				if (isValidDataNum(s+i,))
					addDataNumber(atoi(s+i));
			}
		}
		else
		{
			lerror("Invalid character", line);
			return FALSE_RETURN;
		}
	}
	return TRUE_RETURN;
}