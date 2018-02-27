
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

/* isSymbol:	returns 1 if valid symbol */
int isSymbol(line *l)
{
	return l->flags&VALID_SYM&SYMBOL;
}
