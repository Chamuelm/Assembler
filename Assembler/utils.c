#include <ctype.h>
#include <stdlib.h>
#include "assembler.h"


/* removeColon:	Removes colon from label */
void removeColon(char *symbolName)
{
	int n;

	n = strlen(symbolName);
	symbolName[n-1] = '\0';
}

/* checkSymbol:	Check label for possible errors:
 *  - Length higher than MAX_LABEL_LEN
 * 	-	First character is not a alphabetic letter
 * 	- Contains invalid characters (non alphanumeric)
 * 	- Language keyword
 * 	- Already exist
 *
 * 	Returns error code if invalid otherwise returns NON_ERROR
 *  */
enum errorsShort checkSymbol(char *symbol)
	{
	int length;
	int i = 0, isValid = 1;

	length = strlen(symbol);
	if (!length)															/* missing label name */
		return ERR_LABEL_EMPTY;
	else if (length > MAX_LABEL_LEN)					/* label length */
		return ERR_LABEL_LONG;
	else if (isKeyword(label))								/* Check if label is keyWord */
		return ERR_LABEL_KEYWORD;
	else if (!isalpha(label[0]))							/* starts with non-alpha char */
	 return ERR_LABEL_LETTER;
	else
	{
		for (i=1; i < length; i++)								/* Check if contains invalid chars */
		{
			if (!isalnum(label[i]))
			return ERR_LABEL_CHAR;
		}
	}

		return NON_ERROR;
}


/* getLineType: Returns line type:
 * COMMAND/DATA/STRING/STRUCT/ENTRY/EXTERN/UNKNOWN
 */
enum lineTypes getLineType(char *cmd)
{
	if (getCommand(cmd) != NULL)
		return COMMAND;
	else if(strcmp(cmd, ".data") == 0)
		return DATA;
	else if(strcmp(cmd, ".string") == 0)
		return STRING;
	else if(strcmp(cmd, ".struct") == 0)
		return STRUCT;
	else if(strcmp(cmd, ".entry") == 0)
		return ENTRY;
	else if(strcmp(cmd, ".extern") == 0)
		return EXTERN;
	else
		return UNKNOWN;
}

/* getCommand: check if cmd is known CPU commands.
 * Returns pointer to command or NULL if unknown command
 */
command *getCommand(char *cmd)
{
	int idx;
	command *c = NULL;

	for (idx = 0; idx < CPU_COMMANDS_NUM; idx++)
		if (strcmp(CPUCommands[idx].name, cmd) == 0)
			c = &CPUCommands[idx];

	return c;
}


/* isSymbol:	returns TRUE if symbol declaration */
int isSymbol(char *s)
{
	int n = strlen(s);
	if (s[n-1] == ':')
		return TRUE;
	else
		return FALSE;
}

/* isRegister:	check if s is register name */
int isRegister(char *s)
{
	int num;

	if (strlen(p)n != 2)	/* If length is not 2 it is not register name */
		return FALSE;
	else if (p[0] != 'r')	/* First char must be 'r' */
		return FALSE;
	else if (p[1] > '7' || p[1] < '0')	/* Second char must be digit 0-7 */
		return FALSE;
	else
		return TRUE;
}



/* isValidNum: Check if string s is valid number
 * Prints to stderr if is not valid */
enum errorsShort isValidNum(char *s)
{
	int i, count;
	count = i = 0;

	/* Skip sign */
	if (s[i] == '+' || s[i] == '-')
	{
		i++;
		if (s[i] == '\0')	/* sign without number */
			return ERR_PARAM_SIGN;
	}

	while (s[i] != '\0')
	{
		if(isdigit(s[i]))
			i++;
		else	/* Invalid chracter */
			return ERR_PARAM_CHAR;
	}

	i = atoi(s);
	if(i<=MAX_NUMBER && i >= MIN_MUNBER)		/* Check if number is in hardware's range */
		return NON_ERROR;
	else
		return ERR_PARAM_BIG;
}

/* Check cmd parameter s if it is struct */
int isStruct(char *s)
{
	int i = 0;

	/* Check for dot */
	while(s[i] != '\0')
	{
		if (s[i] == '.')
			break;
		i++;
	}

	if (s[i] == '\0') /* Not struct */
		return FALSE;
	else	/* s[i] == '.' */
	{
		if ((s[i+1] != '\0') &&							/* Check if in string limit */
				(s[i+1] == '1' || s[i+1] == '2') &&		/* Check if call for struct member */
				(s[i+2] == '\0'))						/* Check if no invalid chars after struct */
			return TRUE;
	}
	return FALSE;
}


enum errorsShort checkStructName(char *s)
{
	int n;

	n = strlen(s);	/* Get string lengthe */

	/* Ignore struct member access ie. .1 or .2
	 * to receive symbol name to check validity */
	s[n-2] = '\0';

	/* Check symbol name validity */
	if(checkSymbol(s) != NON_ERROR)
		return ERR_STRUCT_NAME;
	else
		return NON_ERROR;
}

/* calcInstructions:	Calculate number of instructions
 * based on operators left in line
 */
int calcInstructions(instruction *inst)
{
	enum addressingType op1Type, op2Type;
	int i = 1;

	op1Type = inst->op1->type;
	op2Type = inst->op20>type;

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













