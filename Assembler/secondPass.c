/* 
 * File:    secondPass.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains functions related to second pass of the assembler on input file
 * 
 */
#include <stdio.h>
#include "assembler.h"

/* secondPass: process first pass on current active file:
 *  1.  For each line: if is entry check if exist and mark
 *      correct symbol as entry.
 *  2.  Pass on instructions array (instArr) and check each of its
 *      operands are valid.
 * Note: lot of error check and reporting is mmising as 
 *       it dont in first pass.
 */
void secondPass() {
    int lineC;                      /* Line counter */
    enum errorsShort err;           /* Stores error codes */
    enum lineTypes lType;           /* Stores line type */
    char *word;                     /* Temporary word pointer */
    line *currLine;                 /* Temporary line pointer */
    symbol *sym;                    /* Temporary symbol pointer */
    
    /* Line and file initialization */
    fseek(fp, 0, SEEK_SET);         /* Return to start of file */
    lineC = 0;                      /* Line counter initialize */
    currLine = NULL;
    
    /* Processing each line */
    for (;;)
    {   
        lineC++;
        currLine = lineInit(currLine, lineC);
        if (!fgets(currLine->data, MAX_LINE_LEN, fp))
            break;  /* Exit from loop if no more lines */
        
        /******************** Process first word in line ********************/
        word = getWord(currLine);
        if (!word) /* Empty line */
            continue;	/* Skip to next line */
        
        /* if it is a symbol declaration ignore and
         * get next word in line
         */
        if (isSymbol(word))
        {         
            free(word);                 /* Release dynamically allocated memory */
            word = getWord(currLine);   /* Receive next word in line to process*/
            if(!word)	/* Skip line if no more words in line */
                continue;		/* Continue to next line */
        }
        
        /******************** Actual line processing ********************/
        lType = getLineType(word);
        
        /* If is entry 
         * Skip other line types as there is 
                 * nothing more to process */
        if (lType == ENTRY)
            procEntry2(currLine);
        
        /* Memory release */
        free(word);
        free(currLine);
    } /* End of antry lines check */
    
    /* Check for instructions operands validity */
    checkInstArr();
}


void procEntry2(line *l) {
    char *s;                /* Temporary string */
    symbol *sym;            /* Temporary symbol pointer */
    enum errorsShort err;   /* Temporary error code */
    
    if (checkComma(l)) {  /* Check for comma before parameters */
            lerror(ERR_COMMA_BEFOR_PARAM, l->lineNum);
            return;
    }
    
    /* Get at least 1 parameter */
    if (!(s = getParameter(l)))
        lerror(ERR_MISS_PARAM, l->lineNum);
    else
    {
        if ((err = checkEntry(s)) != NON_ERROR) /* Mark as entry if valid */
            lerror(err, l->lineNum);            /* otherwise print error code */
        
        free(s);
    }
    
    for(;;) { /* Get more entry parameters */
        if (checkComma(l)) {
            l->i++;
            if (s = getParameter(l)) {  /* Received parameter seperated with comma */
                if ((err = checkEntry(s)) != NON_ERROR) /* Mark as entry if valid */
                    lerror(err, l->lineNum);            /* otherwise print error code */
                free(s);
            } else {      /* Received comma without parameter */
                lerror(ERR_MISS_PARAM, l->lineNum);
                break;
            }
        } else if (s = getParameter(l)) {   /* Recieved parameter without seperating comma */
            lerror(ERR_COMMA_EXPECTED, l->lineNum);
            if ((err = checkEntry(s)) != NON_ERROR) /* Mark as entry if valid */
                lerror(err, l->lineNum);            /* otherwise print error code */
            free(s);
        }
        else { /* no more input in line */
            break;
        }
    }
}

/* checkInstArr: Check instrutions in instArr if their operands are valid and exist */
void checkInstArr() {
    int i;
    
    for(i=0; i<instIndex; i++){
        if (!checkOperandName(instArr[i]->op1))
            lerror(ERR_VAR_NOT_EXIST, instArr[i]->lineNum);
        if (!checkOperandName(instArr[i]->op2))
            lerror(ERR_VAR_NOT_EXIST, instArr[i]->lineNum);
    }
}

/* checkOperandname: returns TRUE if op name is existing variable */
int checkOperandName(operand *op) {
    
    if (!op)    /* No operator */
        return TRUE;
    
    /* Check if operand name is existing variable */
    if (symLookup(op->data))
        return TRUE;
    else
        return FALSE;
}