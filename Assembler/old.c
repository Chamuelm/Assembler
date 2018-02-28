/* Flags for use in line read functions */
#define COMMA					1			/* Received comma at end of word */
#define EOL						2			/* Received '\n' at end of word */
#define VALID_SYM			4			/* Symbol is valid */
#define SYMBOL				8			/* Line have symbol */
#define LINE_TOO_LONG	16		/* Line is too long */



/*
 * List of instruction types used to indicate the type of current
 * instruction in instructions table:
 * 0	COMMAND				Contains basic command data such as CPU operation,
 * 							source and destination addressing type and encoding type
 * 1	NUM					Contains source and destination register numbers and encoding type
 * 2 	LABEL_ADDRESS		Contains label address (8-bit) and encoding type
 * 3	NUMBER				Contains number (8-bit) and encoding type
 * 4	STRUCT_ITEM_NUM		Contains integer filled with 1 or 2 for desired struct field
 *
 *
enum instructionType {
	COMMAND, REG_NUM, LABEL_ADDRESS, NUMBER, STRUCT_ITEM_NUM
};
*/

/* Holds instruction entry for instructions table
 typedef struct{
 int type;									 Holds current instruction type
 int encodingType;							 Encoding type

 Instruction type is COMMAND:
 int operation;								 Operation name
 int srcAddType;								 Source operand addressing type
 int destAddType;							 Destination operand addressing type

 Instruction type is REG_NUM
 int srcReg;                                  Source registry number (1-8)
 int destReg;								 Destination registry number (1-8)

 For use by instruction type:
 * COMMAND 				not in use
 * REG_NUM				not in use
 * LABEL_ADDRESS		contains the label address
 * NUMBER				contains the number
 * STRUCT_ITEM_NUM	 	contains the item number (1 or 2)

 int val;								 Number in direct addressing

 }instNode;

 Holds data entry for data table
 typedef struct{
 int data;   								Data entry
 int encodingType;							Encoding type
 }dataNode;*/

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


/*
 * getRegisterNum:	Returns register number if is register
 * otherwize returns  FALSE_RETURN;
 */
int getRegisterNum(char *p)
{
	int n, num;

	n = strlen(p);					/* Get name length */

	if (n != 2)								/* If length is not 2 it is not register name */
		num = FALSE_RETURN;
	else if (p[0] != 'r')	/* First char must be 'r' */
		num = FALSE_RETURN;
	else if (p[1] > '7' || p[1] < '0')	/* Second char must be digit 0-7 */
		num = FALSE_RETURN;
	else
		num = p[1]-'0';			/* Find reg number */

	return num;
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
			return FALSE;
		}
	}

	if(checkData(atoi(s)))		/* Check if number is in hardware's range */
		return TRUE;
	else
	{
		lerror("Data parameter is too big.", line);
		return FALSE;
	}
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
/* resetEOL: set EOL flag in l to off */
void resetEOL(line *l)
{
	l->flags &= ~EOL;
}

