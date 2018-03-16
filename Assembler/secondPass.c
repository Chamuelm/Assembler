/* 
 * File:    secondPass.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains functions related to second pass of the assembler on input file
 * 
 */
#include <stdlib.h>
#include "include/assembler.h"

/***************** Internal Functions Declerations ********************/
void procEntry2(line *l);
void checkInstArr();

/********************** Functions Definitions *************************/

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
    enum lineTypes lType;           /* Stores line type */
    char *word;                     /* Temporary word pointer */
    line *currLine;                 /* Temporary line pointer */
    
    /* Line and file initialization */
    fseek(fp, 0, SEEK_SET);         /* Return to start of file */
    lineC = 0;                      /* Line counter initialize */
    currLine = NULL;
    
    /* Processing each line */
    for (;;) {
        lineC++;
        currLine = lineInit(lineC);
                
        if (!fgets(currLine->data, MAX_LINE_LEN, fp)){  /* End of file */
            free(currLine);
            break;  /* Exit from loop if no more lines */
        } else if (!isCompleteLine(currLine)) { /* Reached max line len */
            free(currLine);
            skipEnd(); /* Reach to end of current invalid line in file  */
            continue;
        }
        
        
        
        /******************** Process first word in line ********************/
        word = getWord(currLine);
        if (!word) { /* Empty line */
            free(word);
            free(currLine);
            continue;	/* Skip to next line */
        }
        
        /* if it is a symbol declaration ignore and
         * get next word in line
         */
        if (isSymbol(word))
        {
            free(word);                 /* Release dynamically allocated memory */
            word = getWord(currLine);   /* Receive next word in line to process*/
            if(!word) {	/* Skip line if no more words in line */
                free(currLine);
                continue;		/* Continue to next line */
            }
        }
        
        /******************** Actual line processing ********************/
        lType = getLineType(word);
        
        /* If is entry - check if parameters are internal variable
         * and mark entry existance flag
         */
        if (lType == ENTRY) {
            procEntry2(currLine);
            entryExist = TRUE;
        }
        
        /* If is extern - mark external existance flag */
        if (lType == EXTERN)
            externExist = TRUE;
        
        /* Skip other line types as there is
         nothing more to process */
        
        /* Memory release */
        free(word);
        free(currLine);
    } /* End of entry lines check */
    
    /* Check for instructions operands validity 
     and add externals to externals table */
    checkInstArr();
}


void procEntry2(line *l) {
    char *s;                /* Temporary string */
    error err;   /* Temporary error code */
    
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
            if ((s = getParameter(l))) {  /* Received parameter seperated with comma */
                if ((err = checkEntry(s)) != NON_ERROR) /* Mark as entry if valid */
                    lerror(err, l->lineNum);            /* otherwise print error code */
                free(s);
            } else {      /* Received comma without parameter */
                lerror(ERR_MISS_PARAM, l->lineNum);
                break;
            }
        } else if ((s = getParameter(l))) {   /* Recieved parameter without seperating comma */
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

/* checkInstArr: Check instrutions in instArr if their operands are valid and exist 
 *                      Also add externals to externals table
 */
void checkInstArr() {
    int i, addtionalWordNum;
    symbol *symb;
    
    for(i=0; i<instIndex; i++){
        
        if(instArr[i].op1) {    /* Check only if operand exist */
            if (!checkOperandName(instArr[i].op1)) /* Check if varaiable exist in symbol's table */
                lerror(ERR_VAR_NOT_EXIST, instArr[i].lineNum); /* Error if not exist */
            else if (instArr[i].op1->type == DIRECT) { /* If direct addresing type - check if is external */
                symb = symLookup(instArr[i].op1->data);
                if (symb->address == EXTERNAL_ADDRESS) { /* if is external, add to externals table */
                    addtionalWordNum = 1;    /* Number of additional word is 1 for first operand */
                    addExternal(symb, (instArr[i].memAddress)+addtionalWordNum);
                }
            }
            
            /* Same check for second operand */
            if (instArr[i].op2) {
                if (!checkOperandName(instArr[i].op2))
                    lerror(ERR_VAR_NOT_EXIST, instArr[i].lineNum);
                else if (instArr[i].op2->type == DIRECT) { /* If direct addresing type - check if is external */
                    symb = symLookup(instArr[i].op2->data);
                    if (symb->address == EXTERNAL_ADDRESS) { /* if is external, add to externals table */
                        
                        /* Additional word number calculate */
                        if (instArr[i].op1->type==STRUCT_ADD)
                            /* First operand addressing type is struct and it has 2 additional words */
                            addtionalWordNum = 3;  
                        else
                            /* Number of additional word is 2 for second operand if  first operand is not struct */
                            addtionalWordNum = 2;    
                        
                        /* Add external to table */
                        addExternal(symb, (instArr[i].memAddress)+addtionalWordNum);
                    }
                }
            }
        }
    }
}

