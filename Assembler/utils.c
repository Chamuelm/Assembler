#include <ctype.h>
#include <stdlib.h>
#include "assembler.h"

/* lerror:	Print error message to stderr and increase error counter */
void lerror(char *s, int lineNum)
{
	fprintf(stderr, "Error, line %d: %s.\n", lineNum, s);
	errorDetected++;
}

/* lwarning: Print warning message to stderr */
void lwarning(char *s, int lineNum)
{
	fprintf(stderr, "Warning, line %d: %s.\n", lineNum, s);
}

/* lineInit: Initialize line structure
 * Returns dynamically allocated pointer */
line *lineInit(int lineNumber)
{
	line *lp;

	lp = (line *)malloc(sizeof(line));
	if(!lp)
		exitMemory();

	lp->symbolAddress = 0;
	lp->i = 0;
	lp->lineNum =lineNumber;
	lp->lineType = UNKNOWN;

	return lp;
}

/* exitMemory:	exit the program if memory allocation was unsuccessful */
void exitMemory()
{
	fprintf(stderr, "Not enough memory. Exiting...\n");
	exit(EXIT_FAILURE);
}

/* getWord: returns dynamically allocated string
 * with the next word in line separated by spaces or tabs */
char *getWord(line *l)
{
	int idx, count;
	char *s, *newS;

	skipWhite(l);
	s = l->data;		/* For bright and clear code */
	idx = l->i;
	count = 0;			/* Counter for allocation size */

	while(s[idx] != '\n')
	{
		if(s[idx] == ' ' || s[idx] == '\0')	/* Stops if reached white space */
			break;
		else				/* Count string length */
		{
			idx++;
			count++;
		}
	}

	if (!count)			/* If current char in line is '\n'  after */
		return NULL;	/* skipping white spaces, so no more input in line */
	else
	{
		newS = (char *)malloc(sizeof(char)*(count+1)); /* +1 for '\0' */
		if (!newS)
			exitMemory();
		else
		{
			strncpy(newS, s+(l->i), count);
			newS[count] = '\0';
		}
	}
	l->i += count;	/* increment actual index saved in line struct */
	return newS;
}


/* skipWhite: Skips white spaces and tabs in line */
void skipWhite(line *l)
{
	char c;

	while((c = l->data[l->i]) != '\n')
		if (c == ' ' || c == '\t')
			l->i++;
		else
			break;
}

/* 
 * sSkipWhite: Recieve string s and pointer to index i
 * and increment i to the first non-white space in s
 * Assumes i is in valid range of s
 */
void sSkipWhite(char *s, int *i)
{
	while (s[*i] == '\t' || s[*i] == ' ')
		(*i)++;
}

/* 
 * dismiss White:	Receive string and removes white spaces
 * before and at the end of the string
 */
char *dismissWhite(char *s)
{
	int n,i,j;
	char *new;
	
	n = strlen(s);
	while (n)
	{							/* Decrement last char if it is white */
		if (s[n-1] == ' ' || s[n-1] == '\t')
			n--;
		else
			break;
	}
		
	i = 0;
	while(i < n)
	{							/* Increment i to skip whitespaces */
		if (s[i] == ' ' || s[i] == '\t')
			i++;
		else
			break;
	}
	
	new = malloc(sizeof(char)*(n-i+1));
	if (!new)
	{
		printf(stderr, "Error: Not enough memory. Exiting...\n");
		exit(EXIT_FAILURE);
	}
	
	/* Copy relevant string to return */
	for (j=0; j<(n-1); j++)
		new[j] = s[i+j];
	new[j] = '\0';
	
	return new;
}

/* isSymbol:	returns TRUE_RETURN if symbol declaration */
int isSymbol(char *s)
{
	int n = strlen(s);
	if (s[n-1] == ':')
		return TRUE_RETURN;
	else
		return FALSE_RETURN;
}

/* checkEmpty: Check if line is empty and proceed to first non-blank char
 * returns 1 if empty line otherwise returns 0 */
int checkEmpty(line *l)
{
	skipWhite(l);
	if (l->data[l->i] == '\n')
		return 1;
	else
		return 0;
}

/* resetEOL: set EOL flag in l to off */
void resetEOL(line *l)
{
	l->flags &= ~EOL;
}

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
command *getCommand(char cmd)
{
	int idx;
	command *c = NULL;

	for (idx = 0; idx < CPU_COMMANDS_NUM; idx++)
		if (strcmp(CPUCommands[idx].name, cmd) == 0)
			c = &CPUCommands[idx];

	return c;
}

/* isEOL:		Check for possibilities of end of line and returns the value. */
int isEOL(line *l)
{
	if (l->i == -1)										/* If reached end of line before */
		l->flags |= EOL;
	else if (l->i >= MAX_LINE_LEN)		/* If reached maximum line length */
		l->flags |= EOL;
	else
	{
		skipWhite(l);
		if (l->data[l->i] == '\n')			/* if reached \n */
			l->flags |= EOL;
	}

	return (l->flags & EOL);
}



/* 
 * getRegisterNum:	Returns register number if is register
 * otherwize returns  FALSE_RETURN;
 */
int getRegisterNum(char *p)
{
	int n, num;
	char *name;
	
	name = dismissWhite(p);		/* Remove white spaces if exists (Dynamic alloc)*/
	n = strlen(name);					/* Get name length */
	
	if (n != 2)								/* If length is not 2 it is not register name */
		num = FALSE_RETURN;
	else if (name[0] != 'r')	/* First char must be 'r' */
		num = FALSE_RETURN;
	else if (name[1] > '7' || name[1] < '0')	/* Second char must be digit 0-7 */
		num = FALSE_RETURN;
	else
		num = name[1]-'0';			/* Find reg number */
	
	free(name);								/* Memory release */
	
	return num;
}

/* getParameter:	returns pointer dynamically allocated with the next parameter */
char *getParameter(line *l)
{
	int count;
	char c, *s;

	skipWhite(l);
	count = 0;
	while((c = l->data[(l->i)+count]) != ',' && c != ' ' && c != '\t' && c != '\n')
		count++;

	if (count)
	{
		s = (char *)malloc(sizeof(char)*(count + 1));	/* +1 for '\0' */
		if (!s)
			exitMemory();	/* Exit if memory allocation failed */

		strncpy(s, (l->data)+(l->i), count); /* Copy parameter */
		s[count] = '\0';

		(l->i) += count;										/* Increment line index */

		return s;
	}

	return NULL;
}


int checkComma(line *l)
{
	skipWhite(l);
	if (l->data[l->i] == ',')
		return TRUE_RETURN;
	else
		return FALSE_RETURN;
}

/* isValidNum: Check if string s is valid number
 * Prints to stderr if is not valid */
int isValidNum(char *s, int line)
{
	int i, count;
	count = i = 0;

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

	while (s[i] != '\0')
	{
		if(isdigit(s[i]))
			i++;
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
