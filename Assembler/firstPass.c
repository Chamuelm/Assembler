/* 
 * File:    firstPass.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains functions related to first pass of the assembler on input file
 * 
 */
#include "include/assembler.h"
#include <stdlib.h>
#include <string.h>

/***************** Internal Functions Declarations ********************/
void procCommand(line *l, char *word);
void addData(int val);
void procData(line *l);
void procString(line *l);
void procStruct(line *l);
void procExtern(line *l);
char *removeColon(char *symbolName);

/********************** Functions Definitions *************************/

/* firstPass: process first pass on current active file 
 * This function check each input line and perform validity checks for symbol 
 * and instructions. If line is valid - continue process in seperate functions.
 * 
 * Notes:
 *  -   If line is not a valid assembly line, skipLine flag will be marked
 *      as true to skip process. (therefore there are checks !skipLine in code)
 *  -   Exit from line processing loop is with 'operation' flag
 *  -   Error codes are described in errors.h file
 *  -   Other constant definitions are located in main header file: assembler.h
 *  -   Current active file is in global variable fp.
 *  -   All global variables (but symbols and externals tables) are defined in main.c
 */
void firstPass() {
	int lineC; /* Line counter - will be sent to lerror for error reporting */
	int operation; /* Loop exit variable */
	int skipLine; /* Skip line process flag */
	error err; /* Stores error codes */
	enum lineTypes lType; /* Stores line type */
	char *word; /* Temporary word pointer */
	line *currLine; /* Temporary line pointer */
	int isValidSymbol; /* Symbol flag */
	symbol *sym; /* Symbol pointer */

	/* Processing each line */
	for (operation = TRUE, lineC = 1; operation; lineC++) {
		/* Variables initialization */
		isValidSymbol = 0;
		sym = NULL;
		skipLine = FALSE;
		currLine = lineInit(lineC);

		/* Actual line copy to line struct */
		if (!fgets(currLine->data, MAX_LINE_LEN, fp)) {
			/* End of file
			 * Release memory and stop loop operation */
			free(currLine);
			operation = FALSE;
			skipLine = TRUE;
		} else if (feof(fp)) { /* Reach EOF */
			operation = FALSE;		/* Stop operation after that line */
		} else if (!skipLine && !isCompleteLine(currLine)) {
			/* Current line is longer than maximum allowed
			 * Print relevant error message and release memory */
			lerror(ERR_LINE_LEN, lineC);
			free(currLine);
			skipLine = TRUE;

			/* If line is not a complete line, skip to end of current invalid line
			 * so next loop irritation line read won't include irrelevant data */
			seekEOL(fp);
		}

		/******************** Process first word in line ********************/
		if (!skipLine) {
			/* Copy first word in line to temp (dynamically allocated) pointer */
			word = getWord(currLine);

			/* Line checks before processing */
			if (!word) {
				/* Empty line
				 * Release memory and skip process current line */
				free(currLine);
				skipLine = TRUE;
			} else if (word[0] == ';') {
				/* Comment line
				 * Release memory and skip process current line */
				free(word);
				free(currLine);
				skipLine = TRUE;
			} else if (word[0] == ',') {
				/* Unexpected comma before command or symbol
				 * Print relevant error message, release memory
				 * and skip operation for current line */
				lerror(ERR_COMMA_ENEXPECTED, currLine->lineNum);
				free(word);
				free(currLine);
				skipLine = TRUE;
			} /* End of line checks */
		} /* End of !skipLine block */

		/******************** Symbol check and process ********************/
		/* if it is a symbol declaration - check symbol
		 * validity, add to symbols table and get next word in line */
		if (!skipLine && isSymbol(word)) {
			word = removeColon(word); /* Remove colon sign */

			/* Check symbol name validity */
			if ((err = checkSymbol(word)) == NON_ERROR) {
				/* If symbol name is valid mark relevant flag and add
				 * to symbols table */
				isValidSymbol = 1;
				sym = addSymbol(word, EXTERNAL_ADDRESS, UNKNOWN);
			} else {
				/* If symbol is invalid - mark relevant flag and print error message */
				isValidSymbol = 0;
				lerror(err, lineC);
			}

			free(word); /* Release dynamically allocated memory */
			word = getWord(currLine); /* Get next word in line to process*/
			if (!word) {
				/* No more words in line -> label in empty line
				 * Print relevant error message and skip operation for current line */
				lwarning(ERR_LABEL_EMPTY_LINE, lineC);
				free(currLine);
				skipLine = TRUE;
			} else if (word[0] == ',') {
				/* Unexpected comma before command or symbol
				 * Print relevant error message and stop operation for current line */
				lerror(ERR_COMMA_ENEXPECTED, currLine->lineNum);
				free(word);
				free(currLine);
				skipLine = TRUE;
			} /* End of next word get and checks */
		} /* End of symbol process */

		if (!skipLine) {
			/******************** Actual line processing ********************/
			/* Get line type: COMMAND/DATA/STRING/STRUCT/ENTRY/EXTERN/UNKNOWN */
			lType = getLineType(word);

			/***************** Symbol data update if needed *****************/
			if (isValidSymbol) {
				switch (lType) {
				case COMMAND:
					sym->type = COMMAND;
					sym->address = IC;
					break;
				case DATA:
				case STRING:
				case STRUCT:
					sym->type = DATA;
					sym->address = DC;
					break;
				case ENTRY:
					/* Print warning if label attached to entry
					 and remove from symbol table */
					lwarning(ERR_LABEL_ENTRY, lineC);
					removeSymbol(sym);
					break;
				case EXTERN:
					/* Print error if label attached to entry
					 and remove from symbol table */
					lerror(ERR_LABEL_EXTERN, lineC);
					removeSymbol(sym);
					break;
				case UNKNOWN:
					/* Remove symbol from table if unknown command */
					removeSymbol(sym);
					break;
				default:
					break;
				} /* End of switch */
			} /* End of symbol data update */

			/******************* Continue line processing *******************/
			switch (lType) {
			case COMMAND:
				/* Continue process line in separate function */
				procCommand(currLine, word);
				break;
			case DATA:
				/* Continue process line in separate function */
				procData(currLine);
				break;
			case STRING:
				/* Continue process line in separate function */
				procString(currLine);
				break;
			case STRUCT:
				/* Continue process line in separate function */
				procStruct(currLine);
				break;
			case ENTRY:
				/* Entry line process in second pass */
				break;
			case EXTERN:
				/* Continue process line in separate function */
				procExtern(currLine);
				break;
			case UNKNOWN:
				/* Unknown command
				 * Print relevant error message and skip operation for current line */
				lerror(ERR_UNKNOWN_CMD, lineC);
				break;
			default:
				break;
			} /* End of switch */

			/* Memory release */
			free(word);
			free(currLine);
		} /* End of line processing */
	} /* End of for loop */

	/* Update symbols address:
	 * Add IC to data symbols, and MEMORY_START_ADDRESS to all internal symbols */
	symbolsAddressAdd(IC);

} /* End of firstPass function */

/* procCommand: Processing of command lines:
 *	-	Check for errors in line: Unexpected comma, parameter existence,
 *			invalid argument, invalid addressing type for command.
 *	-	Receive command and operands and add to instructions table.
 *
 */
void procCommand(line *l, char *word) {
	operatorNode *c;
	operand *op1, *op2;
	error err;
	int isValidOperands = TRUE;
	op1 = op2 = NULL;

	/* Get command (struct) from input string 'word' */
	c = getCommand(word);

	/* Continue process by number of parameters needed */
	switch (c->operatorsNum) {
	case 0:

		/* Check for extra text/parameter/comma after operands */
		if ((err = checkEOL(l)) != NON_ERROR) {
			if (err == ERR_MORE_PARAM0)
				/* checkEOL return ERR_MORE_PARAM0 for every line input so we will change it to
				 * ERR_MORE_PARAM0 because our instruction received no parameter */
				lerror(ERR_MORE_PARAM0, l->lineNum);
			else
				lerror(err, l->lineNum); /* Report for other errors */
		} else {
			/* No errors */
			addInstruction(c, NULL, NULL, l->lineNum);
		}

		break;

	case 1: /* Get one operands */
		/* Check comma before parameters */
		if (checkComma(l)) {
			lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
			l->i++;
		}

		op1 = getOperand(l, TRUE);
		if (!op1)
			lerror(ERR_MISS_PARAM, l->lineNum); /* Missing operand */
		else if (op1->type != EMPTY) { /* Received valid operand */

			/* Check for operand addressing type validity */
			err = checkCommandAddType(c, op1, NULL);
			if (err != NON_ERROR) {
				lerror(err, l->lineNum);
				isValidOperands = FALSE;

				/* Check for extra text/parameter/comma after operands */
			} else if ((err = checkEOL(l)) != NON_ERROR) {
				if (err == ERR_MORE_PARAM0)
					/* checkEOL return ERR_MORE_PARAM0 for every line input so we will change it to
					 * ERR_MORE_PARAM1 because our instruction received 1 parameter */
					lerror(ERR_MORE_PARAM1, l->lineNum);
				else
					lerror(err, l->lineNum); /* Report for other errors */

				isValidOperands = FALSE;
			} else
				/* No errors */
				addInstruction(c, op1, NULL, l->lineNum);
		} else
			/* op1->type == empty */
			isValidOperands = FALSE;
		break;

	case 2: /* Get 2 operands */
		/* Check comma before parameters */
		if (checkComma(l)) {
			lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
			l->i++;
		}
		op1 = getOperand(l, TRUE);
		if (!op1)
			lerror(ERR_MISS_PARAM, l->lineNum); /* Missing operand */
		else if (op1->type != EMPTY) { /* Received valid operand */
			if (!checkComma(l)) {
				/* Missing comma */
				lerror(ERR_COMMA_EXPECTED, l->lineNum);
			} else
				l->i++; /* Skip comma */

			op2 = getOperand(l, TRUE);
			if (!op2) {
				/* Missing ssecond operand */
				lerror(ERR_MISS_PARAM, l->lineNum);
				isValidOperands = FALSE;
			} else if (op2->type != EMPTY) { /* Received valid operand */
				/* Check for operand addressing type validity */
				err = checkCommandAddType(c, op1, op2);
				if (err != NON_ERROR) {
					lerror(err, l->lineNum);
					isValidOperands = FALSE;
				} else if ((err = checkEOL(l)) != NON_ERROR) {
					/* Check for extra text/parameter/comma after operands */
					if (err == ERR_MORE_PARAM0)
						/* checkEOL return ERR_MORE_PARAM0 for every line input so we will change it to
						 * ERR_MORE_PARAM2 because our instruction received 2 parameters */
						lerror(ERR_MORE_PARAM2, l->lineNum);
					else
						lerror(err, l->lineNum); /* Report for other errors */

					isValidOperands = FALSE;
				} else { /* Add instruction */
					addInstruction(c, op1, op2, l->lineNum);
				}
			} else { /* op2->type == empty */
				isValidOperands = FALSE;
			}
		} else
			/* op1->type == empty */
			isValidOperands = FALSE;
		break;
	} /* End switch */

	/* Memory release if need */
	if (!isValidOperands) {
		if (op1)
			freeOperand(op1);
		if (op2)
			freeOperand(op2);
	}
} /* End function */

/*
 * addData:	Utility function for procData. Add given number and put in data array
 * Given number is already in 2's complement so no need to convert.
 * Increase DC.
 *
 * Valid data number is in 10 bits, so sign bit should be the 10th bit which
 * is not the case in regular integer, so here we manually turn on or off the sign bit.
 * */
void addData(int val) {
	if (DC < MAX_INSTRUCTIONS) { /* Check for space in data array */
		if (val > 0) /* Turn sign bit off */
			dataArr[DC++] = val & (~SIGN_BIT_MASK);
		else
			/* Turn sign bit on */
			dataArr[DC++] = val | SIGN_BIT_MASK;
	} else {
		fprintf(stderr, "Data array is too small. Exiting...\n");
		exit(EXIT_FAILURE);
	}
}

/* procData:	Process of data line: Check for errors and stores
 * data in data table (number),
 */
void procData(line *l) {
	char *p; /* Temp string pointer */
	error err; /* error type holder */

	skipWhite(l);

	/* Check for comma before data */
	if (l->data[l->i] == ',') {
		lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
		l->i++;
	}

	/* Receive tokens separated by commas */
	p = strtok((l->data) + (l->i), ",");
	while (p) {
		err = isValidNum(p);
		if (err == NON_ERROR)
			addData(atoi(p));
		else
			lerror(err, l->lineNum);

		/* Get next token */
		p = strtok(NULL, ",");
	}
}

/* procString:	Process of string line: stores string in data table,
 * increment DC, and check for errors
 */
void procString(line *l) {
	char c;
	skipWhite(l);

	c = l->data[l->i++]; /* Get next non-white character */
	if (c == '\"') { /* Look for start of string */
		/* Add chars to data table until end of string */
		while (((c = l->data[l->i++]) != '\n') && (c != '\"'))
			addData(c);

		if (c == '\n') { /* Error if not received closing quotation mark */
			lerror(ERR_STR_CLOSE, l->lineNum);
		} else { /* c == '\"' */
			addData('\0'); /* End of string sign - add to data table */
		}

	} else { /* No string opening */
		if (c == '\n') /* No string parameter */
			lerror(ERR_STR_MISS, l->lineNum);
		else
			/* String is not in quotation marks */
			lerror(ERR_STR_QMARK, l->lineNum);
	}
}

/* procStruct:	Process struct line: check for errors and
 * store struct in data table */
void procStruct(line *l) {
	char *temp;
	error err;

	/* Check for unexpected comma before parameter */
	if (checkComma(l)) {
		lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
		l->i++;
	}

	/******* get struct's number *******/
	temp = getParameter(l);
	if (!temp) { /* Missing parameter */
		lerror(ERR_MISS_PARAM, l->lineNum);
		return;
	}

	if ((err = isValidNum(temp)) == NON_ERROR) { /* check number validity */
		addData(atoi(temp)); /* Add struct's number to data array */
		free(temp); /* Temp is dynamically allocated */
	} else
		lerror(err, l->lineNum); /* Prints error if temp is not valid number */

	/* Look for comma between parameters */
	if (checkComma(l))
		l->i++;
	else
		lerror(ERR_COMMA_EXPECTED, l->lineNum);

	/* Process struct's string */
	procString(l);
}

/* procExtern:	Process extern line: check for errors and stores externs in data table */
void procExtern(line *l) {
	char *str;
	error err;

	/* Check for invalid comma before parameter */
	if (checkComma(l)) {
		lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
		l->i++;
	}

	/* get at lease one extern variable */
	str = getParameter(l);
	if (!str) /* No parameter */
		lerror(ERR_MISS_PARAM, l->lineNum);
	else {
		do {
			err = checkSymbol(str);
			if (err == NON_ERROR)
				addSymbol(str, EXTERNAL_ADDRESS, EXTERN);
			else
				lerror(err, l->lineNum);

			free(str);
		} while ((str = getParameter(l)));
	}
}

/* removeColon:	Return new string of label without ending colon sign */
char *removeColon(char *symbolName) {
	int n;

	n = strlen(symbolName);
	symbolName = (char *) realloc(symbolName, sizeof(char) * n);
	if (!symbolName) { /* Allocation check */
		exitMemory();
	} else {
		symbolName[n - 1] = '\0';
	}

	return symbolName;
}

/* checkCommandAddType:     Check operands addressing type validity for command c
 * Check is made by allowedSrc and allowedDest of command c.
 * allowed Src and allowedDest represents in bits their allowed addressing type:
 *  IMMEDIATE = 1, DIRECT = 2, STRUCT_ADD = 4, REGISTER = 8
 * 
 * Therefore, checks like (c->allowedSrc & op1->type) will be TRUE if operand type is one of
 * the allowed by command.
 */
enum errorsShort checkCommandAddType(operatorNode *c, operand *op1,
		operand *op2) {
	if (c->allowedSrc) { /* command c receive 2 operands */
		if (!op1 || !op2) /* Missing operand */
			return ERR_MISS_PARAM;
		else {
			if (!(c->allowedSrc & op1->type))
				return ERR_SRCADD_TYPE;

			if (!(c->allowedDest & op2->type))
				return ERR_DESTADD_TYPE;
		}
	} else if (c->allowedDest) { /* Command c receive one operand */
		if (!op1) /* Missing operand */
			return ERR_MISS_PARAM;
		else {
			if (!(c->allowedDest & op1->type))
				return ERR_DESTADD_TYPE;
		}
	}
	return NON_ERROR;
}
