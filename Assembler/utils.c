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
#include <string.h>
#include "include/assembler.h"


/* checkSymbol:	Check label for possible errors:
 *  - Length higher than MAX_LABEL_LEN
 * 	- First character is not a alphabetic letter
 * 	- Contains invalid characters (non alphanumeric)
 * 	- Language keyword
 * 	- Already exist
 *
 * 	Returns error code if invalid otherwise returns NON_ERROR
 *  */
error checkSymbol(char *symbol) {
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
        for (i=1; i < length; i++)  {	/* Check if contains invalid chars */
            if (!isalnum(symbol[i]))
                return ERR_PARAM_CHAR;
        }
    }
    
    return NON_ERROR;
}




/* getCommand: check if cmd is known CPU commands.
 * Returns pointer to command or NULL if unknown command
 */
operatorNode *getCommand(char *comm) { 
    int idx, operation;
    operatorNode *c = NULL;

    operation = TRUE;
    for (idx = 0; idx < CPU_COMMANDS_NUM && operation; idx++)
    	if (strcmp(CPUCommands[idx].name, comm) == 0) {
    		c = (operatorNode *)&CPUCommands[idx];
    		operation = FALSE;
    	}
    
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

/* isRegister:	check if s is register name */
int isRegister(char *s) {
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
error isValidNum(char *s) {
    int i = 0;
    
    /* Skip white spaces before number  if exist */
    while(s[i] == ' ' || s[i] == '\t')
        i++;
    
    /* Skip sign if exist */
    if (s[i] == '+' || s[i] == '-') {
        i++;
        if (!isdigit(s[i]))	/* sign without number */
            return ERR_PARAM_SIGN;
    }
    
    while (s[i]!='\0' && s[i]!=' ' && s[i]!='\t' && s[i]!='\n') {
        if(isdigit(s[i]))
            i++;
        else  /* Invalid chracter */
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



/* checkStructName: check if s is valid struct name */
error checkStructName(char *s) {
    int n;
    
    n = strlen(s);	/* Get string length */
    
    /* Ignore struct member access ie. .1 or .2
     * to receive symbol name to check validity */
    s[n-2] = '\0';
    
    /* Check symbol name validity */
    if(checkSymbol(s) != NON_ERROR)
    {
        s[n-2] = '.';
        return ERR_STRUCT_NAME;
    } else {
        s[n-2] = '.';
        return NON_ERROR;
    }
    
    s[n-2] = '.';  /* Retrive membre access */
}


/* strdcat:     Concatanate addition to dest and take care for memory allocation. 
 *              Returned value is dynamically allocated and have to be freed later.
 */
char *strdcat(char *dest, const char *addition) {
    int newStringLen;
    char *newString;
    
    /* Determine lengths */
    newStringLen = strlen(dest);        /* Add length of dest */
    newStringLen += strlen(addition);   /* Add length of addition */
    newStringLen += 1;                  /* Add space for '\0' */
    
    /* Memory allocation */
    newString = (char *)malloc(sizeof(char)*newStringLen);
    if (!newString)
        exitMemory();   /* Exit if memory allocation failed */
    
    /* Strings copy and concatenade */
    strcpy(newString, dest);
    strcat(newString, addition);
    
    return newString;
}

/* strdup: Copy string s to returned new allocated space */
char *strdup(char *s) {
    char *p;
    p = (char *) malloc(strlen(s) + 1);
    if(p != NULL)
        strcpy(p, s);
    return p;
}

/* checkEntry:  check if s is not external var and mark as entry
 return NON_ERROR if s is valid entry otherwise return error code */
error checkEntry(char *s) {
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

/* hash:	form hash value for string s */
unsigned int hash(char *s) {
    unsigned int hashval;
    
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31*hashval;
    return hashval % HASHSIZE;
}

/* skipEnd:       Seek file pointer untill find '\n'
 * Reads one character each time and check for '\n' or EOF */
void skipEnd(FILE *fileP) {
    char c = 0;
    
    while(c != '\n' && c != EOF)
        fread(&c, sizeof(char), 1, fileP);
}