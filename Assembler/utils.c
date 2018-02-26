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
