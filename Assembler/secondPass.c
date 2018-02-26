/* procCommand: process command line */
void procCommand(line *l, char *cmdName)
{
	command *c;
	operand o1, o2;

	c = checkCommand(cmdName);
	if (c == NULL)
	{
		lerror("Unknown command", l->lineNum);
		return;
	}

	if (!getOperands(l, &o1, &o2))
		return;				/* Finish process if operands are not valid */

	if (c->operatorsNum == 2)
	{


	}
}

/* getOperand: received line and index in line and return the next operand */
int getOperands(line *l, operand *o1, operand *o2)
{
	char op1[MAX_LINE_LEN], op2[MAX_LINE_LEN], *c;
	int isValid = 1;
	op1[0] = op2[0] = '\0';

	skipWhite(l);
	if (l->data[l->i] == ',')
	{
		lerror("Invalid comma before operand", l->lineNum);
		isValid = 0;
		l->i++;
	}

	c = strtok((l->data)+(l->i), ",");		/* Divide by ',' to tokens */
	if (!c)		/* No operands */
	{
		o1->type = o2->type = EMPTY;
		o1->data[0] = o2->data[0] = '\0';
	}
	else
	{
		strcpy(o1->data, c);
		o1->type = getOperandType(c);

		c = strtok(NULL, ",");
		if(!c)
		{
			o2->type = EMPTY;
			o2->data[0] = '\0';
		}
		else
		{
			strcpy(o2->data, c);
			o2->type = getOperandType(c);

			if (strtok(NULL, ","))
			{
				lerror("Too many operators", l->lineNum);
				isValid = 0;
			}
		}
	}

	return isValid;
}
