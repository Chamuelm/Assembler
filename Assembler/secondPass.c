/* 
 * File:    secondPass.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains functions related to second pass of the assembler on input file
 * 
 */
#include <stdlib.h>
#include <string.h>
#include "include/assembler.h"

/***************** Internal Functions Declarations ********************/
void procEntry2(line *l);
void procCommand2(line *l, char *word);
symbol *checkVarExist(char *s);

/********************** Functions Definitions *************************/

/* secondPass: Process second pass on current active file:
 *  1.  For each line: if is entry check if exist and mark
 *      relevant symbol as entry.
 *  2.  Pass on instructions array (instArr) and check each of its
 *      operands are exist (if variable).
 *  3.	Add externals to externals table.
 * 
 * Note:	Most of error checks are done in first pass
 *	so these checks are not reported here again
 *	and in case of failure its just skip the line.
 */
void secondPass() {
	int lineC; /* Line counter - will be sent to lerror for error reporting */
	int operation; /* Loop exit variable */
	int skipLine; /* Skip line process flag */
	enum lineTypes lType; /* Stores line type */
	char *word; /* Temporary word pointer */
	line *currLine; /* Temporary line pointer */

	/* Line and file initialization */
	fseek(fp, 0, SEEK_SET); /* Return to start of file */
	lineC = 0; /* Line counter initialize */
	currLine = NULL;

	/* Initialize instructions counter for second pass */
	IC = 0;

	/* Processing each line */
	for (operation = TRUE, lineC = 1; operation; lineC++) {
		/* Variables initialization */
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
			 * Skip to next and release memory */
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
				 * Release memory and skip operation for current line */
				free(word);
				free(currLine);
				skipLine = TRUE;
			} /* End of line checks */
		} /* End of !skipLine block */

		/******************** Symbol check and process ********************/
		/* if it is a symbol declaration - skip to next word in line */

		if (!skipLine && isSymbol(word)) {
			free(word);
			word = getWord(currLine); /* Get next word in line to process*/
			if (!word) {
				/* No more words in line -> label in empty line
				 * Skip operation for current line */
				free(currLine);
				skipLine = TRUE;
			} else if (word[0] == ',') {
				/* Unexpected comma before command or symbol
				 * Skip operation for current line */
				free(word);
				free(currLine);
				skipLine = TRUE;
			} /* End of next word get and checks */
		} /* End of symbol process */

		if (!skipLine) {
			/******************** Actual line processing ********************/
			/* Get line type: COMMAND/DATA/STRING/STRUCT/ENTRY/EXTERN/UNKNOWN */
			lType = getLineType(word);

			/******************* Continue line processing *******************/
			switch (lType) {
			case COMMAND:
				/* Continue process line in separate function:
				 * Check for operands existence and add to externals table
				 * if uses external variable */
				procCommand2(currLine, word);
				break;
			case ENTRY:
				/* Continue process line in separate function:
				 * Check if line parameters are exist internal variables
				 * and mark them as entry. */
				procEntry2(currLine);

				/* Mark global variable for existence of entries in the program */
				entryExist = TRUE;
				break;
			case EXTERN:
				/* Mark global variable for existence of externals in the program */
				externExist = TRUE;
				break;

				/* Skip other line types as there is nothing else to do */
			case UNKNOWN:
			case DATA:
			case STRING:
			case STRUCT:
			default:
				break;
			} /* End of switch */

			/* Memory release */
			free(word);
			free(currLine);
		} /* End of line processing */
	} /* End of for loop */
} /* End of secondPass function */

/* procEntry2:		Process entry line in second pass: check if parameters
 * are exist internal symbols and mark symbol as entry.
 */
void procEntry2(line *l) {
	char *s; /* Temporary string */
	error err; /* Temporary error code */
	int operation; /* Loop exit variable */

	if (checkComma(l)) { /* Check for comma before parameters */
		lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
		return;
	}

	/* Get at least 1 parameter */
	if (!(s = getParameter(l)))
		lerror(ERR_MISS_PARAM, l->lineNum);
	else {
		if ((err = checkEntry(s)) != NON_ERROR) /* Mark as entry if valid */
			lerror(err, l->lineNum); /* otherwise print error code */

		free(s);
	}

	/* Get more entry parameters if exist */
	operation = TRUE;
	while (operation) {
		if (checkComma(l)) {
			l->i++;
			if ((s = getParameter(l))) { /* Received parameter separated with comma */
				if ((err = checkEntry(s)) != NON_ERROR) /* Mark as entry if valid */
					lerror(err, l->lineNum); /* otherwise print error code */
				free(s);
			} else { /* Received comma without parameter */
				lerror(ERR_MISS_PARAM, l->lineNum);
				operation = FALSE; /* Exit from loop */
			}
		} else { /* No comma */
			if ((s = getParameter(l))) { /* Received parameter without separating comma */
				lerror(ERR_COMMA_EXPECTED, l->lineNum);
				free(s);
			}
			operation = FALSE; /* Exit from loop */
		}
	}
}

/* procCommand2: Processing command line in second pass:
 * Check for variable existence and add externals to externals table
 *
 * All line validity checks are done again like in first pass because we need
 * to maintain the instructions counter (IC).
 * IC is used to update externals table.
 */
void procCommand2(line *l, char *word) {
	operatorNode *c;
	operand *op1, *op2;
	error err;
	symbol *sym;
	int addtionalWordNum;
	instruction tempInst;

	op1 = op2 = NULL;

	/* Get command (struct) from input string 'word' */
	c = getCommand(word);

	/* Continue process by number of parameters needed */
	switch (c->operatorsNum) {
	case 0: /* No need to check operands as they aren't any */

		/* Check for extra text/parameter/comma after operator */
		if ((err = checkEOL(l)) == NON_ERROR) {
			/* Increment IC for later use */
			IC += 1; /* Command with one operand is one code instruction */
		}
		break;

	case 1:

		/* Check comma before parameters and skip it if exist */
		if (checkComma(l))
			l->i++;

		op1 = getOperand(l, FALSE);
		/* Continue only if line has been processed in first pass */
		if (op1 && (op1->type != EMPTY)
				&& (checkCommandAddType(c, op1, NULL) == NON_ERROR)
				&& (checkEOL(l) == NON_ERROR)) {

			/* Check for variable existence and if external */
			if (op1->type & LABEL_ADD) { /* If is direct or struct addressing type */
				if ((sym = checkVarExist(op1->data))) {
					if (sym->type == EXTERN) {
						addtionalWordNum = 1; /* First additional word of instruction */
						addExternal(sym,
								IC + addtionalWordNum + MEMORY_START_ADDRESS);
					}
				} else {
					/* Variable does not exist */
					lerror2s(ERR_VAR_NOT_EXIST, op1->data, l->lineNum);
				}
			}
			tempInst.cmd = c;
			tempInst.op1 = op1;
			tempInst.op2 = NULL;
			IC += calcInstructions(&tempInst);
		}
		break;

	case 2: /* Get 2 operands */
		/* Check comma before parameters and skip it if exist */
		if (checkComma(l))
			l->i++;

		op1 = getOperand(l, FALSE);
		/* Continue only if line has been processed in first pass */
		if ((op1) && (op1->type != EMPTY)) {

			/* Skip comma if exist. First pass process line even if comma is missing */
			if (checkComma(l))
				l->i++;

			op2 = getOperand(l, FALSE);
			/* Continue only if line has been processed in first pass */
			if (op2 && (op2->type != EMPTY)
					&& (checkCommandAddType(c, op1, op2) == NON_ERROR)
					&& (checkEOL(l) == NON_ERROR)) {

				/* Check for variables existence and if externals */
				if ((op1->type & LABEL_ADD)) { /* If is direct or struct addressing type */
					if ((sym = checkVarExist(op1->data))) {
						if (sym->type == EXTERN) {
							addtionalWordNum = 1; /* First additional word of instruction */
							addExternal(sym,
									IC + addtionalWordNum + MEMORY_START_ADDRESS);
						}
					} else {
						/* Variable does not exist */
						lerror2s(ERR_VAR_NOT_EXIST, op1->data, l->lineNum);
					}
				}

				if ((op2->type & LABEL_ADD)) { /* If is direct or struct addressing type */
					if ((sym = checkVarExist(op2->data))) {
						if (sym->type == EXTERN) {

							if (op1->type == STRUCT_ADD)
								addtionalWordNum = 3; /* Struct have 2 additional word so we skip 2 */
							else
								addtionalWordNum = 2; /* Second additional word of instruction */

							addExternal(sym,
									IC + addtionalWordNum + MEMORY_START_ADDRESS);
						}
					} else {
						/* Variable does not exist */
						lerror2s(ERR_VAR_NOT_EXIST, op2->data, l->lineNum);
					}
				}
				tempInst.cmd = c;
				tempInst.op1 = op1;
				tempInst.op2 = op2;
				IC += calcInstructions(&tempInst);
			}
		}
		break;

	} /* End switch */

	/* Memory release if need */
		if(op1)
			freeOperand(op1);
		if(op2)
			freeOperand(op2);

} /* End of function: procCommand2 */



/* checkVarExist:  Check if s exist in symbols table and returns it's symbol */
symbol *checkVarExist(char *s) {
	int n;
	symbol *sym;

	if (isStruct(s)) {
		n = strlen(s);
		s[n - 2] = '\0'; /* Hide struct member to get 'pure' name */
		sym = symLookup(s);
		s[n - 2] = '.'; /* Retrieve full data */
	} else
		sym = symLookup(s);

	return sym;
}
