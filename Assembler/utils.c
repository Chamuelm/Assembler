#include <ctype.h>
#include "assembler.h"

/* lerror:	Print error message to stderr and increase error counter */
void lerror(char *s, int lineNum)
{
	fprintf(stderr, "Error in line %d: %s.\n", lineNum, s);
	errorDetected++;
}

/* lwarning: Print warning message to stderr */
void lwarning(char *s, int lineNum)
{
	fprintf(stderr, "Warning! line %d: %s.\n", lineNum, s);
}

/* lineInit: Initialize line structure */
void lineInit(line *lp, int lineNumber)
{
	symbolAddress = i = flags = 0;
	lineNum = lineNumber;
	lineType = UNKNOWN;
}

/* getWord: Copy the next word in line to word
 * Words seperated by white spaces or commas	*/
int getWord(line *l, char *word)
{
	int operation, index;
	char c;
	index = 0;
	operation = 1;

	resetEOL(l);			/* Reset EOL flag */
	skipWhite(l);
	while((l->flags&(EOL|WORD_TOO_LONG|LINE_TOO_LONG) != 0) && operation)
	{


		c = l->data[l->i];
		switch (c)
		{
		case '\n':
			l->i == -1;
			*flags |= EOL;		/* flag end of line */
			operation = 0;
			break;
		case ',':
			*flags |= COMMA;	/* flag for comma */
			operation = 0;
			break;
		case ' ': case '\t':
			operation = 0;
			break;
		default:
			word[index++] = c;
			l->i++;
			break;
		}

		if (l->i == MAX_LINE_LEN)			/* Check if exceeded max line length */
			l->flags |= LINE_TOO_LONG;
	}

	if (flags |= LINE_TOO_LONG)
		index--;
	word[index] = '\0';
	return index;
}

/* skipWhite: Skips white spaces and tabs in line */
void skipWhite(line *l)
{
	while(l->i < MAX_LINE_LEN)
		if (!(l->data[l->i] == ' ' || l->data[l->i] == '\t'))
			return;
		else
			l->i++;
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

/* isLabel: check whether word is label or not
 * Remove colon sign if needed and reduce length to right length */
int isLabel(line *l, char *word, int *index)
{
	if (word[(*index)-1] == ':')
	{
		(*index)--;											/* Reduce length after to remove colon sign */
		word[*index] = '\0';						/* Remove colon sign */
		l->flags |= SYMBOL;
		return 1;
	}
	else
		return 0;
}

/* checkLabel:	Check label for errors as described belowe */
int checkLabel(line *l, char *label, int length) {
	int i = 0, isValid = 1;
	if (length == 1)													/* Error - Solo colon sing */
	{
		lerror("Empty label", l->lineNum);
		isValid = 0;
	}
	else if (length > MAX_LABEL_LEN)					/* Check if label is too long. */
	{
		lerror("Lavel invalid: length exceeded maximum allowed");
		isValid = 0;
	}
	else if (isKeyword(label))								/* Check if label is keyWord */
	{
		lerror("Label cannot be a keyword", l->lineNum);
		isValid = 0;
	}

	if (l->flags & EOL)												/* Label with no data */
	{
		lwarning("Label is not attached to instruction or data", l->lineNum);
		isValid = 0;
	}

	if (!isalpha(label[0]))										/* Check if starts with non-alpha char */
	{
		lerror("Label invalid: must start with letter", l->lineNum);
		isValid = 0;
	}
	for (i=1; i < length; i++)								/* Check if contains invalid chars */
	{
		if (!isalnum(label[i]))
		{
			lerror("Label invalid: contains non-alphanumeric characters.", l->lineNum);
			isValid = 0;
			break;
		}
	}

	if (!isValid)									/* Set flag to invalid if needed */
		l->flags |= INVALID_SYM;

	return isValid;
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

/* isSymbol:	returns 1 if valid symbol */
int isSymbol(line *l)
{
	return l->flags&VALID_SYM;
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


