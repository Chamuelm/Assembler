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

/***************** Internal Functions Declerations ********************/
void procCommand(line *l, char *word);
void addData(int val);
void procData(line *l);
void procString(line *l);
void procStruct(line *l);
void procExtern(line *l);
char *removeColon(char *symbolName);
enum errorsShort checkCommandAddType(operatorNode *c, operand *op1, operand *op2);


/********************** Functions Definitions *************************/

/* firstPass: process first pass on current active file */
void firstPass() {
    int lineC;                      /* Line counter */
    int operation;              /* Loop exit variable */
    int skipLine;                   /* Skip line process flag */
    error err;                      /* Stores error codes */
    enum lineTypes lType;   /* Stores line type */
    char *word;                 /* Temporary word pointer */
    line *currLine;                 /* Temporary line pointer */	
    int isValidSymbol;              /* Symbol flag */
    symbol *sym;                    /* Symbol pointer */
    
    /* Processing each line */
    for (skipLine=FALSE, operation = TRUE, lineC=1, currLine=NULL; operation; lineC++) {
        isValidSymbol = 0;
        sym = NULL;
        
        /* Line struct initialization and memory allocation */
        currLine = lineInit(lineC);
        
        /* Actual line copy to line struct */
        if (!fgets(currLine->data, MAX_LINE_LEN, fp)){ 
            /* End of file 
             * Release memory and stop loop operation */
            free(currLine);
            operation = FALSE;
            skipLine = TRUE;
        }
        if (!skipLine && !isCompleteLine(currLine)) {
            /* Currnt line is longer than maximum allowed
             * Print relevant error message and release memory */
            lerror(ERR_LINE_LEN, lineC);
            free(currLine);
            skipLine = TRUE;
            
            /* If line is not a complete line, skip to end of current invalid line
             * so next loop irritation line read won't include irrelevant data */
            skipEnd(fp); 
        }
        
        /******************** Process first word in line ********************/
        
        /* Copy first word in line to temp (dynamically allocated) pointer */
        word = getWord(currLine); 
        
        /* Line checks before processing */
        if (!word) {
            /* Empty line 
             * Release memory and skip process current line */
            free(currLine);
        } else if (word[0] == ';') {
            /* Comment line 
             * Release memory and skip process current line */
            free(word);
            free(currLine);
        } else if (word[0] == ',') {
            /* Unexpected comma before command or symbol 
             * Print relevant error message, release memory
             * and skip operation for current line */
            lerror(ERR_COMMA_ENEXPECTED,  currLine->lineNum);
            free(word);
            free(currLine);
        } else { /* Continue process line */
            
            /******************** Symbol check and process ********************/
            
            /* if it is a symbol declaration - check symbol
             * validity, add to symbols table and get next word in line */
            if (isSymbol(word)) {
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
                    lerror(err,lineC);
                }
                
                free(word);                             /* Release dynamically allocated memory */
                word = getWord(currLine);      /* Get next word in line to process*/
                if(!word) {
                    /* No more words in line -> label in empty line 
                     * Print relevant error message and skip operation for current line */
                    lwarning(ERR_LABEL_EMPTY_LINE, lineC);
                    free(currLine);
                } else if (word[0] == ',') {
                    /* Unexpected comma before command or symbol 
                     * Print relevant error message and stop operation for current line */
                    lerror(ERR_COMMA_ENEXPECTED,  currLine->lineNum);
                    free(word);
                    free(currLine);
                } /* End of next word get and checks */
            } /* End of symbol process */
            
            /******************** Actual line processing ********************/
            /* Get line type: COMMAND/DATA/STRING/STRUCT/ENTRY/EXTERN/UNKNOWN */
            lType = getLineType(word);
            
            /***************** Symbol data update if needed *****************/
            if(isValidSymbol) {
                switch(lType) {
                    case COMMAND:
                        sym->type = COMMAND;
                        sym->address = IC;
                        break;
                    case DATA: case STRING: case STRUCT:
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
                        /* Remove symbol if unknown command */
                        removeSymbol(sym);
                    default:
                        break;
                }
            }
            
            /******************* Continue line processing *******************/
            switch (lType) {
                case COMMAND:
                    procCommand(currLine, word);
                    break;
                case DATA:
                    procData(currLine);
                    break;
                case STRING:
                    procString(currLine);
                    break;
                case STRUCT:
                    procStruct(currLine);
                    break;
                case ENTRY:		
                    /* Entry process in second pass */
                    break;
                case EXTERN:
                    procExtern(currLine);
                    break;
                case UNKNOWN:
                    /* Unknown command 
                     * Print relevant error message and skip operation for current line */
                    lerror(ERR_UNKNOWN_CMD, lineC);
                default:
                    break;
            }
            free(word); /* Memory release */
            free(currLine);
        }
    }
}

/* Update symbols address by IC+MEMORY_START_ADDRESS to all data symbols address */
symbolsAddressAdd(IC);
}


void procCommand(line *l, char *word) {
    operatorNode *c;
    operand *op1, *op2;
    error err;
    
    /* Get command (struct) from input string 'word' */
    c = getCommand(word);
    
    
    /* Continue process by number of parameters needed */
    switch(c->operatorsNum) {
	case 0:
            
            /* Check for extra text/parameter/comma after operands */
            if ((err = checkEOL(l)) != NON_ERROR) {
                if (err == ERR_MORE_PARAM0)
                    /* checkEOL return ERR_MORE_PARAM0 for every line input so we will change it to 
                     * ERR_MORE_PARAM0 because our instruction received no parameter */
                    lerror(ERR_MORE_PARAM0, l->lineNum);
                else
                    lerror(err, l->lineNum);    /* Report for other errors */
            } else /* No errors */
                addInstruction(c, NULL, NULL, l->lineNum);
            
            break;
            
            
	case 1:	/* Get one operands */
            /* Check comma before parameters */
            if(checkComma(l)) {
                lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
                l->i++;
            }
            
            op1 = getOperand(l);
            if(!op1)
                lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
            else if (op1->type != EMPTY) { /* Received valid operand */
                
                /* Check for operand addressing type validity */
                err = checkCommandAddType(c, op1, NULL);
                if(err != NON_ERROR)
                    lerror(err, l->lineNum);
                
                /* Check for extra text/parameter/comma after operands */
                if ((err = checkEOL(l)) != NON_ERROR) {
                    if (err == ERR_MORE_PARAM0)
                        /* checkEOL return ERR_MORE_PARAM0 for every line input so we will change it to 
                         * ERR_MORE_PARAM1 because our instruction received 1 parameter */
                        lerror(ERR_MORE_PARAM1, l->lineNum);
                    else
                        lerror(err, l->lineNum);    /* Report for other errors */
                }
                else /* No errors */
                    addInstruction(c, op1, NULL, l->lineNum);
            }
            break;
            
            
	case 2:	/* Get 2 operands */
            /* Check comma before parameters */
            if(checkComma(l)) {
                lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
                l->i++;
            }
            op1 = getOperand(l);
            if(!op1)
                lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
            else if (op1->type != EMPTY) { /* Received valid operand */
                if(!checkComma(l)) {	/* look for comma between parameters */
                    /* Missing comma */
                    op2 = getOperand(l);
                    if(!op2)																/* If no comma and not operand */
                        lerror(ERR_MISS_PARAM, l->lineNum);	/* Report for missing operand */
                    else if (op2->type != EMPTY) { /* Received valid operand */
                        /* Check for operand addressing type validity */
                        err = checkCommandAddType(c, op1, op2);
                        if(err != NON_ERROR)
                            lerror(err, l->lineNum);
                        
                        /* Missing comma between parameters */
                        lerror(ERR_COMMA_EXPECTED, l->lineNum);
                        addInstruction(c, op1, op2, l->lineNum);
                    }
                } else { /* received comma */
                    l->i++; /* Increment line indext to skip comma */
                    op2 = getOperand(l);
                    if(!op2)
                        lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
                    else {	/* Received operand */
                        
                        /* Check for operand addressing type validity */
                        err = checkCommandAddType(c, op1, op2);
                        if(err != NON_ERROR)
                            lerror(err, l->lineNum);
                        else if ((err = checkEOL(l)) != NON_ERROR) {
                            /* Check for extra text/parameter/comma after operands */
                            if (err == ERR_MORE_PARAM0)
                                /* checkEOL return ERR_MORE_PARAM0 for every line input so we will change it to 
                                 * ERR_MORE_PARAM2 because our instruction received 2 parameters */
                                lerror(ERR_MORE_PARAM2, l->lineNum);
                            else
                                lerror(err, l->lineNum);    /* Report for other errors */
                        }
                        else /* No errors */
                            addInstruction(c, op1, op2, l->lineNum);
                    }
                }
            }
            break;
            
            
    }	/* End switch */
}	/* End function */





/*
 * addData:	add given number and put in data array
 * Given number is already in 2's complement so no need to convert.
 * */
void addData(int val) {
    if (DC < MAX_INSTRUCTIONS) { /* Check for space in data array */
        if (val > 0)	/* Turn sign bit off */
            dataArr[DC++] = val&(~SIGN_BIT_MASK);
        else		/* Turn sign bit on */
            dataArr[DC++] = val|SIGN_BIT_MASK;
    } else {
        fprintf(stderr, "Data array is too small. Exiting...\n");
        exit(EXIT_FAILURE);
    }
}



/* procData:	Stores data in data table (number), 
 * increment DC, and check for errors */ 
void procData(line *l) {
    char *p;			/* Temp string pointer */
    error err;           /* error type holder */
    
    skipWhite(l);
    
    /* Check for comma before data */
    if (l->data[l->i] == ',') {
        lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
        l->i++;
    }
    
    /* Receive tokens separated by commas */
    p = strtok((l->data)+(l->i), ",");
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

/* procString:	Stores string in data table, increment DC, and check for errors */
void procString(line *l) {
    char c;
    skipWhite(l);
    
    c = l->data[l->i++];
    if(c == '\"') {	/* Start of string */
        /* Add chars to data table until end of string */
        while ((c = l->data[l->i++]) != '\n') {
            if (c == '\"') {	/* = end of string */
                addData('\0');
                break;
            }
            else 
                addData(c);
        }
        
        if (c == '\n') {	/* Error if not received closing quotation mark */
            lerror(ERR_STR_CLOSE, l->lineNum);
        }
    } else {	/* Error detection */
        if (c == '\n')  /* No string parameter */
            lerror(ERR_STR_MISS, l->lineNum);
        else            /* String is not in qoutation marks */
            lerror(ERR_STR_QMARK, l->lineNum);
    }
}

/* procStruct:	Stores struct in data table, increment DC, and check for errors */
void procStruct(line *l) {
    char *temp;
    error err;
    
    if (checkComma(l)) {		/* Received comma before parameter */
        lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
        l->i++;
    }
    
    temp = getParameter(l);
    if (!temp) {
        lerror(ERR_MISS_PARAM, l->lineNum);
        return;
    }
    
    if ((err=isValidNum(temp))==NON_ERROR) {
        addData(atoi(temp));	/* Add struct's number to data array */
        free(temp);
    }
    else
        lerror(err, l->lineNum); /* Prints error if temp is not valid number */
    
    /* Look for comma between parameters */
    if (checkComma(l))
        l->i++;
    else
        lerror(ERR_COMMA_EXPECTED, l->lineNum);
    
    /* Process struct's string */
    procString(l);
}

/* procExtern:	Stores externs in data table and check for errors */
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
    symbolName = (char *)realloc(symbolName, sizeof(char)*n);
    if (!symbolName) { /* Allocation check */
        exitMemory();
    } else {
        symbolName[n-1] = '\0';
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
enum errorsShort checkCommandAddType(operatorNode *c, operand *op1, operand *op2) {    
    if (c->allowedSrc) { /* command c receive 2 operands */
        if (!op1 || !op2)   /* Missing operand */
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