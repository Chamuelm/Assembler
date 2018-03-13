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


/********************** Functions Definitions *************************/

/* firstPass: process first pass on current active file */
void firstPass() {
    int lineC;                      /* Line counter */
    error err;           /* Stores error codes */
    enum lineTypes lType;           /* Stores line type */
    char *word;                     /* Temporary word pointer */
    line *currLine;                 /* Temporary line pointer */	
    int isValidSymbol;              /* Symbol flag */
    symbol *sym;                    /* Symbol pointer */
    
    /* Line initialize */
    lineC = 0;
    currLine = NULL;
    
    /* Processing each line */
    for (;;) {
        lineC++;
        isValidSymbol = 0;
        sym = NULL;
        
        currLine = lineInit(lineC);
        if (!fgets(currLine->data, MAX_LINE_LEN, fp)){ 
            free(currLine);
            break;  /* Exit from loop if no more lines */
        }
        
        
        /******************** Symbol check processing ********************/
        word = getWord(currLine);
        if (!word) { /* Empty line */
            free(currLine);
            continue;	/* Skip to next line */
        }
        
        /* Check if line is comment */
        if (word[0] == ';') {
            free(word);
            free(currLine);
            continue;
        }
        
        /* if it is a symbol declaration check symbol
         * validity and get next word in line
         */
        if (isSymbol(word)) {
            /* Remove colon sign and release unused memory */
            word = removeColon(word);
            
            if ((err = checkSymbol(word)) == NON_ERROR) {
                isValidSymbol = 1;
                sym = addSymbol(word, EXTERNAL_ADDRESS, UNKNOWN);
            } else {
                isValidSymbol = 0;
                lerror(err,lineC);
            }
            
            free(word);                 /* Release dynamically allocated memory */
            word = getWord(currLine);   /* Receive next word in line to process*/
            if(!word) {	/* No more words in line = label in empty line */
                lwarning(ERR_LABEL_EMPTY_LINE, lineC);
                free(currLine);
                continue;		/* Continue to next line */
            }
        }
        
        /******************** Actual line processing ********************/
        lType = getLineType(word);
        
        if(lType == UNKNOWN) {		/* If unknown command, continue to next line */
            lerror(ERR_UNKNOWN_CMD, lineC);
            free(word);             /* Memory release */
            free(currLine);
            continue;
        }
        
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
            case ENTRY:		/* Skip if entry */
                break;
            case EXTERN:
                procExtern(currLine);
                break;
            default:
                break;
        }
        free(word); /* Memory release */
        free(currLine);
    }
    
    /* Update symbols address by IC to all external symbols address */
    symbolsAddressAdd(DC);
}


void procCommand(line *l, char *word) {
    operatorNode *c;
    operand *op1, *op2;
    c = getCommand(word);
    
    /* Continue process by number of parameters needed */
    switch(c->operatorsNum) {
	case 0:
            /* Check number of parameters */
            if(!isEOL(l)) {
                lerror(ERR_MORE_PARAM0, l->lineNum);
                return;
            } else
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
            else {	/* Other line validity checks */
                if (checkComma(l)) {
                    if (getOperand(l))
                        lerror(ERR_MORE_PARAM1, l->lineNum); /* Too many parameters */
                    else
                        lerror(ERR_COMMA_EXTRA, l->lineNum); /* Extra comma after parameter */
                } else {
                    if (isEOL(l))
                        addInstruction(c, op1, NULL, l->lineNum);
                    else
                        lerror(ERR_CMD_EXTRA_TEXT, l->lineNum);
                }
                
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
            else {
                if(!checkComma(l)) {	/* look for comma between parameters */
                    l->i++;
                    op2 = getOperand(l);
                    if(!op2)	/* If no comma and not operand */
                        lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
                    else {
                        lerror(ERR_COMMA_EXPECTED, l->lineNum);
                        addInstruction(c, op1, op2, l->lineNum);
                    }
                } else { /* received comma */
                    l->i++;
                    op2 = getOperand(l);
                    if(!op2)
                        lerror(ERR_MISS_PARAM, l->lineNum);	/* Missing operand */
                    else	/* No errors */
                        addInstruction(c, op1, op2, l->lineNum);
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
    
    if (checkComma(l)) {
        lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
        l->i++;
    }
    while ((str = getParameter(l))) {
        err = checkSymbol(str);
        if (err == NON_ERROR)
            addSymbol(str, EXTERNAL_ADDRESS, EXTERN);
        else
            lerror(err, l->lineNum);
        
        free(str);
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