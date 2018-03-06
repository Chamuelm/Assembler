/*
 * output.c
 *
 *  Contains output files creation functions
 *      Author: Moshe hamiel
 *      ID:		308238716
 */
#include "assembler.h"
#include <stdio.h>
#include <string.h> 

/* Create output files of active file */
void createOutputFiles() {
    /* object file creation - need instructions array */
    createObjectsFile();
    
    /* extern symbol file creation - need symbols array */
    /* entry file creation - need labels array */
}

void createObjectsFile() {
    FILE *ob;
    char *obFileName;
    char base32String1[BASE_32_LEN+1]; /* +1 for '\0' */
    char base32String2[BASE_32_LEN+1];
    int i;
    int binCode;
    
    /* Determine object file name */
    obFileName = malloc(sizeof(char)*((strlen(fileName))+OB_SUFFIX_LEN));
    if (!obFileName)
        exitMemory();   /* memory allocation failed */
    obFileName = strcat(obFileName, ".ob"); /* Add suffix to name */
    
    /* Open object file */
    ob = fopen(obFileName, "w+");
    if (!ob) { /* If fopen fails print error and stop process */
        fprintf(stderr, "Error: cannot create file %s: %s\n", obFileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    /* Add IC and DC to object file */
    binToBase32(IC, base32String1);
    binToBase32(DC, base32String2);
    fAddLine(ob, base32String1, base32String2);
    
    /* For each instruction call fAddInst to add to file */
    for (i=0; i< instIndex; i++) {
        fAddInst(ob, instArr[i]);
        
        
        /*
         binToBase32(binCode, instArr[i].)
         
         binCode = instToBin(instArr[i]);
         binToBase32(binCode, base32String);
         */
    }
    
    fclose(ob);
}

/*  binToBase32: Convert num to base 32 representation as string
 *  and store result in dest 
 *  Works only with 10 first 10 bits of num
 *  and need dest's length to be at least 3 to store result.
 */
void binToBase32(int num, char *dest) {
    int x;
    
    /* Get first character */
    x = num&BITS_MASK_10BITS; /* Ignore irrelevant bits */
    x = x >> 5;
    dest[0] = base32Digit[x];
    
    /* Get second character */
    x = num&BITS_MASK_5BITS; /* Ignore irrelevant bits */
    dest[1] = base32Digit[x];
    
    /* Add '\0' to end of string */
    dest[2] = '\0';
}

/*
 *  fAddLine:  Add str1 and str2 to file ob in the format:
 *              str1 \t str2 \n
 */
void fAddLine(FILE *fileP, char *str1, char *str2) {
    if (fprintf(fileP, "%s\t%s\n", str1, str2) < 0) {
        fprintf(stderr, "Error while writing to file: ", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

/*
 * fAddInst:    Adds coded instruction lines of instruction x to file fileP 
 */
void fAddInst(FILE fileP, instruction x) {
    
    
    switch (x.cmd->operatorsNum) { /* Devide prlocess by number of operators */
        case 0:
            fAddInst0op(fileP, x);
            break;
        case 1:
            fAddInst1op(fileP, x);
            break;
        case 2:
            fAddInst2op(fileP, x);
            break;
    }
}

void fAddInst0op(FILE fileP, instruction x) {
    /* Just add command word as there is no additional words */
    fAddCommandWord(fileP, x);
}

void fAddInst1op(FILE fileP, instruction x) {
    int  n;
    enum addressingType opType;
    
    /***** Add first word: command representaion *****/
    fAddCommandWord(fileP, x);    
    
    /***** Add additional words by oprand addressing type *****/
    opType = x->op1->type;  
    
    switch(opType) {
	case IMMEDIATE:
            /* Add additional word with the operand data in the 
             next address after command word */
            fAddImmediateWord(fileP, (x->op1->data)+1, x->addressOffset+1);
                    /* +1 to skip '#' immediate prefix */
            break;
        case DIRECT:
            fAddDirectWord(fileP, x->op1->data, x->addressOffset+1);
            break;
        case REGISTER:
            fAddRegisterWord(fileP, x->op1->data, x->addressOffset+1);
            break;
	case STRUCT_ADD:
            n = strlen(x->op1->data);
            (x->op1->data)[n-2] = '\0'; /* Ignore struct member */
            fAddDirectWord(fileP, x->op1->data, x->addressOffset+1);
            
            (x->op1->data)[n-2] = '.'; /* Retrive struct member data */
            fAddImmediateWord(fileP, (x->op1->data)+(n-1), x->addressOffset+2);
                /* +(n-1) to skip to struct member,
                 * addressOffset+2 as this is the 2nd additional member */
            break;
    }
}



/* 
 * fAddCommandWord: Add command word to fileP
 */
void fAddCommandWord(FILE fileP, instruction x) {
    int addressCode, wordCode;
    char addressSTR[BASE_32_LEN+1]; /* +1 for '\0' */
    char wordSTR[BASE_32_LEN+1];
    
    /***** Add address information *****/
    addressCode = x.addressOffset;  /* Command code's address is the first in offset */
    
    /***** Add command information *****/
    
    /* Add opcode representation to instCode*/
    wordCode = x.cmd->opcode << CODE_OPCODE_LOC;

    /* Add addressing types representations yo instCode */    
    switch (x.cmd->operatorsNum)
    {
        case 0:
            /* No additional words and no need to add information */
            break;
        case 1:
            /* Operand addressing type is destination addressing type so
               add dest addressing type representation to instCode */
            wordCode |= (getAddressingTypeCode(x.op1->type)<<CODE_DEST_OP_LOC);
            break;
        case 2:
            /* Add addressing types representation yo instCode */
            wordCode |= (getAddressingTypeCode(x.op1->type)<<CODE_SRC_OP_LOC);
            wordCode |= (getAddressingTypeCode(x.op2->type)<<CODE_DEST_OP_LOC);
    }
    
    /* No need to update coding type as is always Absolute for commands */
    
    /* Convert to string representation */
    binToBase32(addressCode, addressSTR);
    binToBase32(wordCode, wordSTR);
    
    /* Add new line to fileP */
    fAddLine(fileP, addressSTR, wordSTR);
}

/* 
 * fAddImmediateWord: Add immediate word with given address to fileP
 */
void fAddImmediateWord(FILE *fileP, char *data, int address) {
    char addressSTR[BASE_32_LEN+1]; /* +1 for '\0' */
    char wordSTR[BASE_32_LEN+1];
    
    /* Convert to string representation */
    binToBase32(address, addressSTR);
    binToBase32((atoi(data)<<CODE_DATA_LOC), wordSTR);
    
    /* No need to change coding type for immediate */
    
    /* Add new line to fileP */
    fAddLine(fileP, addressSTR, wordSTR);
}

/* 
 * fAddRegisterWord: Add register word with given address to fileP
 */
void fAddRegisterWord(FILE *fileP, char *data, int address) {
    char addressSTR[BASE_32_LEN+1]; /* +1 for '\0' */
    char wordSTR[BASE_32_LEN+1];
    
    /* Convert to string representation */
    binToBase32(address, addressSTR);
    binToBase32(atoi(data+1)<<CODE_DEST_OP_LOC, wordSTR); /* +1 to skip 'r' register prefix */
    
    /* No need to change coding type for register */
    
    /* Add new line to fileP */
    fAddLine(fileP, addressSTR, wordSTR);
}

/* 
 * fAddDirectWord: Add direct word with given address to fileP
 */
void fAddDirectWord(FILE *fileP, char *data, int address) {
    char addressSTR[BASE_32_LEN+1]; /* +1 for '\0' */
    char wordSTR[BASE_32_LEN+1];
    int wordCode;                   /* Holds code numeric representation */
    int codingType;                 /* Word coding type */
    symbol *sym;                    /* Symbol pointer */
    
    sym = symLookup(data);
    
    /* Get word data */
    wordCode = sym->address<<CODE_DATA_LOC;
    
    /* Get word coding type */
    wordCode |= (sym->address == EMPTY_ADDRESS)?EXTERNAL:RELOCATABLE;
    
    /* Convert to string representation */
    binToBase32(address, addressSTR);
    binToBase32(wordCode, wordSTR);
    
    /* Add new line to fileP */
    fAddLine(fileP, addressSTR, wordSTR);
}


/* 
 * getAddressingTypeCode: Return numeric representation of addresing type
 */
int getAddressingTypeCode(enum addressingType type) {
     switch (type) {
         case IMMEDIATE:
             return 0;
             break;
         case DIRECT:
             return 1;
             break;
         case STRUCT_ADD:
             return 2;
             break;
         case REGISTER:
             return 3;
             break;
         default:
             return 0;
             break;
     }
}