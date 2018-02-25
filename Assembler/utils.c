#include <ctype.h>
#include "assembler.h"

/* getWord: Copy the next word in line to word
 * Words seperated by white spaces or commas
 * Returns number of chars has been copied */
int getWord(char *line, int *index, char *word, int *flags)
{
	int operation, i;
	char c;
	i = 0;
	operation = 1;
	skipWhite(line, index);
	while(index && *index < MAX_LINE_LEN && i < MAX_LABEL_LEN && operation)
	{
		c = line[*index];
		switch (c)
		{
		case '\n':
			index = null;
			*flags |= EOL;		/* flag end of line */
			break;
		case ',':
			*flags |= COMMA;	/* flag for comma */
			operation = 0;
			break;
		case ' ': case '\t':
			operation = 0;
			break;
		default:
			word[i++] = c;
			(*index)++;
			break;
		}
	}
	
	if (i == MAX_LABEL_LEN)		/* Word is too long */
	{
		*flags |= WORD_TOO_LONG;
		word[i-1] = '\0';
		return (i-1);
	}
	
	word[i] = '\0';
	return i;
}

/* skipWhite: Skips white spaces and tabs in line */
void skipWhite(char *line, int *index)
{
	while(index && *index < MAX_LINE_LEN)
		if (!(line[*index] == ' ' || line[*index] == '\t'))
			break;
		else
			(*index)++;
}
}