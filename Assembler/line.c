/*
 * line.c
 *
 *  Contains line manipulation functions
 *      Author: Moshe hamiel
 *      ID:		308238716
 */
#include "include/assembler.h"
#include <stdlib.h>
#include <string.h>


/********************** Functions Definitions *************************/

/* lineInit: Initialize line structure
 * free oldLine if is allocated and returns dynamically
 * allocated pointer */
line *lineInit(int lineNumber) {
    line *lp;
    
    /* New line allocation */
    lp = (line *)malloc(sizeof(line));
    if(!lp)
        exitMemory();   /* Exit if memory allocation failed */
    
    lp->i = 0;
    lp->lineNum =lineNumber;
    
    return lp;
}

/* getWord: returns dynamically allocated string
 * with the next word in line separated by spaces or tabs */
char *getWord(line *l) {
    int idx, count;
    char *s, *newS;
    
    skipWhite(l);
    s = l->data;		/* For bright and clear code */
    idx = l->i;
    count = 0;			/* Counter for allocation size */
    
    while(s[idx] != '\n') {
        if(s[idx] == ' ' || s[idx] == '\0' || s[idx] == '\t')	/* Stops if reached white space */
            break;
        else {				/* Count string length */ 
            idx++;
            count++;
        }
    }
    
    if (!count)			/* If current char in line is '\n'  after */
        return NULL;	/* skipping white spaces, so no more input in line */
    else {
        newS = (char *)malloc(sizeof(char)*(count+1)); /* +1 for '\0' */
        if (!newS)
            exitMemory(); /* Memory allocation failed */
        else {
            strncpy(newS, s+(l->i), count);
            newS[count] = '\0';
        }
    }
    l->i += count;	/* increment actual index saved in line struct */
    return newS;
}

/* getParameter:	returns pointer dynamically allocated with the next parameter */
char *getParameter(line *l) {
    int count;
    char c, *s;
    
    skipWhite(l);
    count = 0;
    while((c = l->data[(l->i)+count]) != ',' && c != ' ' && c != '\t' && c != '\n')
        count++;
    
    if (count) {
        s = (char *)malloc(sizeof(char)*(count + 1));	/* +1 for '\0' */
        if (!s)
            exitMemory();	/* Exit if memory allocation failed */
        
        strncpy(s, (l->data)+(l->i), count); /* Copy parameter */
        s[count] = '\0';
        
        (l->i) += count;		/* Increment line index */
        
        return s;
    }
    
    return NULL;
}

/*  getOperand:	Returns pointer to dynamically allocated operand from line l */
operand *getOperand(line *l) {
    error err;
    char *s, *temp;
    operand *op;
    int invalid = 0;		/* Operand validity flag */
    
    /* Get parameter text if exist */
    if (!(s = getParameter(l)))
        return NULL;
    
    /* Operand struct initialization */
    op = (operand *)malloc(sizeof(operand));
    if(!op)		/* Check memory allocation */
        exitMemory();
    
    /************ Addressing Type: IMMEDIATE ************/
    if (s[0] == '#') {
        /* Skip hashtag (#) sign */
        temp = s;
        s = strdup(s+1);
        if (!s)	/* memory allocation check */
            exitMemory();
        free(temp);	/* Release memory not in use */
        
        if(strlen(s) == 0) {		/* '#' without number */
            lerror(ERR_PARAM_NO_NUM, l->lineNum);
            invalid = 1;
        } else if((err=isValidNum(s)) == NON_ERROR) {	/* Check number validity */
            op->type = IMMEDIATE;
            op->data = s;
            return op;
        } else {		/* if number is not valid report error */
            lerror(err, l->lineNum);
            invalid = 1;
        }
    }
    /************ Addressing Type: REGISTER ************/
    else if(isRegister(s))
    {
        op->data = s;
        op->type = REGISTER;
    }
    /************ Addressing Type: STRUCT ************/
    else if(isStruct(s))
    {
        err = checkStructName(s);
        if(err == NON_ERROR) {
            op->data = s;
            op->type = STRUCT_ADD;
        } else {
            lerror(ERR_STRUCT_NAME, l->lineNum);
            invalid = 1;
        }
    }
    /************ Addressing Type: DIRECT ************/
    else
    {
        err = checkSymbol(s);
        if(err == NON_ERROR) {
            op->data = s;
            op->type = DIRECT;
        } else {
            lerror(ERR_PARAM_NAME, l->lineNum);
            invalid = 1;
        }
    }
    
    if (invalid) {
        op->type = EMPTY;
        op->data = s;
    }
    
    return op;
}

/* skipWhite: Skips white spaces and tabs in line */
void skipWhite(line *l) {
    char c;
    
    while((c = l->data[l->i]) != '\n')
        if (c == ' ' || c == '\t')
            l->i++;
        else
            break;
}

/* isEOL:		Check for end of line */
int isEOL(line *l) {
    skipWhite(l);
    if (l->data[l->i] == '\n')			/* if reached \n */
        return TRUE;
    else
        return FALSE;
}

/* checkComma: Returns true if next non-white character is comma */
int checkComma(line *l) {
    skipWhite(l);
    if (l->data[l->i] == ',')
        return TRUE;
    else
        return FALSE;
}

/* getLineType: Returns line type:
 * COMMAND/DATA/STRING/STRUCT/ENTRY/EXTERN/UNKNOWN
 */
enum lineTypes getLineType(char *cmd) {
    if (getCommand(cmd) != NULL)
        return COMMAND;
    else if(strcmp(cmd, ".data") == 0)
        return DATA;
    else if(strcmp(cmd, ".string") == 0)
        return STRING;
    else if(strcmp(cmd, ".struct") == 0)
        return STRUCT;
    else if(strcmp(cmd, ".entry") == 0)
        return ENTRY;
    else if(strcmp(cmd, ".extern") == 0)
        return EXTERN;
    else
        return UNKNOWN;
}

/* checkEOL:    Check for end of line:
 * return error for extra parameter/extra text/extra comma
 */
error checkEOL(line *l) {
    
    /* Return NON_ERROR if rest of line is white spaces */
    if (isEOL(l))
        return NON_ERROR;
    
    /* Check for comma - if there is comma so check for parameter or return error for extra comma  */
    if (checkComma(l)) {
        l->i++;             /* increment line index to skip comma */
        skipWhite(l);       /* Skip white spaces after comma if exist */
        
        if (l->i < MAX_LINE_LEN && l->data[l->i] != '\n')
            return ERR_MORE_PARAM0; /* Indicate for extra parameter in line */    
        else
            return ERR_COMMA_EXTRA;  /* Indicate for extra comma after parameters */
    } else { /* No comma */
        if (l->i < MAX_LINE_LEN && l->data[l->i] != '\n')
            return ERR_CMD_EXTRA_TEXT;      /* Indicate for extra text after parameters */
        else
            return NON_ERROR;   /* No comma and no extra text after */
    }
}

/* isCompleteLine:  Check if l is full input line by looking for '\n' */
int isCompleteLine(line *l) {
    int i;
    for (i=0; i<MAX_LINE_LEN; i++)
        if (l->data[i] == '\n')
            return TRUE;
    return FALSE;
}