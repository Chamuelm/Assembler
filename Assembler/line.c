/*
 * line.c
 *
 *  Contains line manipulation functions
 *      Author: Moshe hamiel
 *      ID:		308238716
 */
#include "assembler.h"
#include <stdio.h>


/* lineInit: Initialize line structure
 * free oldLine if is allocated and returns dynamically
 * allocated pointer */
line *lineInit(line *oldLine, int lineNumber) {
    line *lp;
    
    /* Release old line if exist */
    if (oldLine != NULL)
        free(oldLine);
    
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
        if(s[idx] == ' ' || s[idx] == '\0')	/* Stops if reached white space */
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
            exitMemory();
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

/* checkEntry:  check if s is not external var and mark as entry
 return NON_ERROR if s is valid entry otherwise return error code */
enum errorsShort checkEntry(char *s) {
    symbol *sym;
    if ((sym = symLookup(s))) {
        /* if symbol found, check if not extern */
        if (sym->type == EXTERN)
            return ERR_EXTERN_ENTRY;
        else {
            sym->isEntry = TRUE;
            return  NON_ERROR;
        }
    }
    else 
        return ERR_VAR_NOT_EXIST;
}


/*  getOperand:	Returns pointer to dynamically allocated operand from line l */
operand *getOperand(line *l) {
    enum errorsShort err;
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
        } else if((err =isValidNum(s)) == NON_ERROR) {	/* Check number validity */
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
        op->type = REG_ADD;
    }
    /************ Addressing Type: STRUCT ************/
    else if(isStruct(s))
    {
        if(checkStructName(s))
            op->data = s;
        op->type = STRUCT_ADD;
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
        op->type = INVALID_ADD_TYPE;
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

/* checkEmpty: Check if line is empty and proceed to first non-blank char
 * returns 1 if empty line otherwise returns 0 */
int checkEmpty(line *l) {
    skipWhite(l);
    if (l->data[l->i] == '\n')
        return 1;
    else
        return 0;
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
