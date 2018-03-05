/* 
 * File:    utils.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains utilities functions such as type check and error check
 * 
 */

#include <ctype.h>
#include <stdlib.h>
#include "assembler.h"


/* removeColon:	Removes colon from label */
void removeColon(char *symbolName) {
    int n;
    
    n = strlen(symbolName);
    symbolName[n-1] = '\0';
}

/* checkSymbol:	Check label for possible errors:
 *  - Length higher than MAX_LABEL_LEN
 * 	- First character is not a alphabetic letter
 * 	- Contains invalid characters (non alphanumeric)
 * 	- Language keyword
 * 	- Already exist
 *
 * 	Returns error code if invalid otherwise returns NON_ERROR
 *  */
enum errorsShort checkSymbol(char *symbol) {
    int length, i = 0;
    
    length = strlen(symbol);
    if (!length)
        return ERR_LABEL_MISSING;       /* Label name missing */
    else if (length > MAX_LABEL_LEN)	
        return ERR_LABEL_LONG;          /* Label exceeded max length */
    else if (isKeyword(symbol))
        return ERR_LABEL_KEYWORD;       /* label is a keyword */
    else if (!isalpha(symbol[0]))
        return ERR_LABEL_LETTER;        /* starts with non-alpha char */
    else {
        for (i=1; i < length; i  {	/* Check if contains invalid chars */
            if (!isalnum(symbol[i]))
                return ERR_LABEL_CHAR;
        }
    }
    
    return NON_ERROR;
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

/* getCommand: check if cmd is known CPU commands.
 * Returns pointer to command or NULL if unknown command
 */
operatorNode *getCommand(char *cmd) { 
    int idx;
    operatorNode *c = NULL;
    
    for (idx = 0; idx < CPU_COMMANDS_NUM; idx++)
        if (strcmp(CPUCommands[idx].name, cmd) == 0)
            c = &CPUCommands[idx];
    
    return c;
}


/* isSymbol:	returns TRUE if symbol declaration */
int isSymbol(char *s) {
    int n = strlen(s);
    if (s[n-1] == ':')
        return TRUE;
    else
        return FALSE;
}

/* isKeyword: returns TRUE if string s is language keyboard */
int isKeyword(char *s) {
    int i = 0;
    
    /* Check if command name */
    if(getCommand(s) != NULL)
        return TRUE;
    
    /* Check if register name */
    if(isRegister(s))
        return TRUE;
    
    /* Check for other keywords */
    while(keywordTab[i] != NULL)
    {
        if (strcmp(keywordTab[i], s) == 0)
            return TRUE;
        i++;
    }
    
    return FALSE;
}

/* isRegister:	check if s is register name */
int isRegister(char *s) {
    int num;
    
    if (strlen(s) != 2)	/* If length is not 2 it is not register name */
        return FALSE;
    else if (s[0] != 'r')	/* First char must be 'r' */
        return FALSE;
    else if (s[1] > '7' || s[1] < '0')	/* Second char must be digit 0-7 */
        return FALSE;
    else
        return TRUE;
}



/* isValidNum: Check if string s is valid number
 * Prints to stderr if is not valid */
enum errorsShort isValidNum(char *s) {
    int i = 0;
    
    /* Skip white spaces before number  if exist */
    while(s[i] == ' ' || s[i] == '\t')
        i++;
    
    /* Skip sign if exist */
    if (s[i] == '+' || s[i] == '-') {
        i++;
        if (s[i] == '\0' || s[i] == ' ' || s[i] == '\t')	/* sign without number */
            return ERR_PARAM_SIGN;
    }
    
    while (s[i]!='\0' && s[i]!=' ' && s[i]!='\t') {
        if(isdigit(s[i]))
            i++;
        else	/* Invalid chracter */
            return ERR_PARAM_CHAR;
    }
    
    /* Check if there is another parameter
     attached without ',' seperating */
    if (s[i] == ' ' || s[i] == '\t') {
        /* Skip white spaces */
        while(s[i] == ' ' || s[i] == '\t')
            i++;
        if (s[i] != '\0')   /* found not comma-seperated parameter */
            return (ERR_COMMA_BEFOR_PARAM);
    }
    
    i = atoi(s);
    /* Check if number is in hardware's range */
    if(i<=MAX_NUMBER && i >= MIN_MUNBER)
        return NON_ERROR;
    else
        return ERR_PARAM_BIG;
}

/* Check cmd parameter s if it is struct */
int isStruct(char *s) {
    int i = 0;
    
    /* Check for dot */
    while(s[i] != '\0') {
        if (s[i] == '.')
            break;
        i++;
    }
    
    if (s[i] == '\0') /* Not struct */
        return FALSE;
    else {	/* s[i] == '.' */
        if ((s[i+1] != '\0') &&				/* Check if in string limit */
                (s[i+1] == '1' || s[i+1] == '2') &&	/* Check if call for struct member */
                (s[i+2] == '\0'))			/* Check if no invalid chars after struct */
            return TRUE;
    }
    return FALSE;
}

/* checkStructName: check if s is valid struct name */
enum errorsShort checkStructName(char *s) {
    int n;
    
    n = strlen(s);	/* Get string lengthe */
    
    /* Ignore struct member access ie. .1 or .2
     * to receive symbol name to check validity */
    s[n-2] = '\0';
    
    /* Check symbol name validity */
    if(checkSymbol(s) != NON_ERROR)
        return ERR_STRUCT_NAME;
    else
        return NON_ERROR;
}



/* calcInstructions:	Calculate number of instructions
 * based on operators left in line
 */
int calcInstructions(instruction *inst) {
    enum addressingType op1Type, op2Type;
    int i = 1;
    
    op1Type = inst->op1->type;
    op2Type = inst->op2->type;
    
    /*
     * Increment i by type of addresing type:
     *	All but struct required 1 additional word
     *	Struct required 2 additional words.
     *	If both are REGISTER they can share the same word
     */
    switch(op1Type) {
	case EMPTY:
            break;
	case REGISTER:
	case IMMEDIATE:
	case DIRECT:
            i++;
            break;
	case STRUCT_ADD:
            i += 2;
    }
    
    switch(op2Type) {
	case EMPTY:
            break;
	case REGISTER:
            if (op1Type != REGISTER)
                i++;	/* If op1Type is REGISTER it use */
            break;	/* the same additional word */
	case IMMEDIATE:
	case DIRECT:
            i++;
            break;
	case STRUCT_ADD:
            i += 2;
    }
    
    return i;
}