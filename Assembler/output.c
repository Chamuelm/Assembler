/*
 * output.c
 *
 *  Contains output files creation functions
 *      Author: Moshe hamiel
 *      ID:		308238716
 */
#include "./include/assembler.h"
#include <stdio.h>
#include <string.h> 

/* Create output files of active file */
void createOutputFiles() {
    
    createObjectsFile();    /* Objects file creation */
    createExtEntFiles();    /* External symbols and entries
                             * files creation (if needed) */
}

/* createObjectFile:    create object file of current active assembly file */
void createObjectsFile() {
    FILE *ob;
    char *obFileName;
    char base32String1[BASE_32_LEN+1]; /* +1 for '\0' */
    char base32String2[BASE_32_LEN+1];
    int i;
    
    /* Determine object file name */
    obFileName = strdcat(fileName, objectFileExtension);
    
    /* Open object file */
    ob = fopen(obFileName, "w+");
    if (!ob) { /* If fopen fails print error and stop process */
        fprintf(stderr, "Error: cannot create file %s: %s\n", obFileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    free(obFileName);
    
    /* Add IC and DC to object file */
    intToBase32STR(IC, base32String1);
    intToBase32STR(DC, base32String2);
    fAddLine(ob, base32String1, base32String2);
    
    /* Add instructions to file */
    for (i=0; i<instIndex; i++) {
        fAddInst(ob, instArr[i]);
    }
    
    /* Add data to file */
    for (i=0; i<DC; i++) {
        /* Address to base-32 string representaion */
        intToBase32STR(MEMORY_START_ADDRESS+IC+i, base32String1);
        /* Data to base-32 string representaion */
        intToBase32STR(dataArr[i], base32String2);
        
        /* Add to file */
        fAddLine(ob, base32String1, base32String2);
    }
    fclose(ob);
}

/*  intToBase32STR: Convert num to base 32 representation as string
 *  and store result in dest 
 *  Works only with first 10 bits of num
 *  and need dest's length to be at least BASE_32_LEN to store result.
 */
void intToBase32STR(int num, char *dest) {
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
 * fAddInst:    Main function to add coded instruction lines 
 * of instruction x to file fileP 
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

/* fAddInst0op: Add coded instruction line of commands 
                with no parameters */
void fAddInst0op(FILE fileP, instruction x) {
    /* Just add command word as there is no additional words */
    fAddCommandWord(fileP, x);
}

/* fAddInst1op: Add coded instruction line of commands 
                with one parameters */
void fAddInst1op(FILE fileP, instruction x) {
    int n;
    
    /***** Add first word: command representaion *****/
    fAddCommandWord(fileP, x);    
    
    /***** Add additional words by oprand addressing type *****/

    switch(x->op1->type) {
	case IMMEDIATE:
            /* Add additional word with the operand data in the 
             next address after command word */
            fAddImmediateWord(fileP, x->op1->data, (x->memAddress)+1);
            break;
        case DIRECT:
            fAddDirectWord(fileP, x->op1->data, x->memAddress+1);
            break;
        case REGISTER:
            fAddRegisterWord(fileP, NULL, x->op1->data, x->memAddress+1);
            /* Command with 1 operand is always dest operand 
                therefore src register is NULL */
            break;
	case STRUCT_ADD:
            n = strlen(x->op1->data);
            (x->op1->data)[n-2] = '\0'; /* Ignore struct member */
            fAddDirectWord(fileP, x->op1->data, x->memAddress+1);
            
            (x->op1->data)[n-2] = '.'; /* Retrive struct member data */
            fAddImmediateWord(fileP, atoi((x->op1->data)+(n-1)), x->memAddress+2);
                /* +(n-1) to skip to struct member,
                 * addressOffset+2 - this is the 2nd additional word */
            break;
    }
}

/* fAddInst2op: Add coded instruction line of commands 
                with 2 parameters */
void fAddInst2op(FILE fileP, instruction x) {
    int n;
    int wordNum = 1;        /* Address offset from command word memory location */
    
    /***** Add first word: command representaion *****/
    fAddCommandWord(fileP, x);    
    
    /***** Add additional words for source operand *****/
    switch(x->op1->type) {
	case IMMEDIATE:
            /* Add additional word with the operand data in the 
             next address after command word */
            fAddImmediateWord(fileP, x->op1->data, (x->memAddress)+wordNum);
            break;
        case DIRECT:
            fAddDirectWord(fileP, x->op1->data, x->memAddress+wordNum);
            break;
        case REGISTER:
            if (x->op2->type == REGISTER) {
                fAddRegisterWord(fileP, x->op1->data, x->op2->data, x->memAddress+wordNum);
                return;
            } else /* Destination operand is not register */
                fAddRegisterWord(fileP, x->op1->data, NULL, x->memAddress+wordNum);
            break;
	case STRUCT_ADD:
            n = strlen(x->op1->data);
            (x->op1->data)[n-2] = '\0'; /* Ignore struct member */
            fAddDirectWord(fileP, x->op1->data, x->memAddress+wordNum);
            wordNum++;  /* Increment word number - struct address is 2 additional words */
            
            (x->op1->data)[n-2] = '.'; /* Retrive struct member data */
            fAddImmediateWord(fileP, atoi((x->op1->data)+(n-1)), x->memAddress+wordNum);
                /* +(n-1) to skip to struct member,
                 * addressOffset+2 - this is the 2nd additional word */
            break;
    }
    wordNum++; /* Increment word number for the next operand 
                * address's addistional words */
    
    /***** Add additional words for destination operand *****/
        switch(x->op2->type) {
	case IMMEDIATE:
            /* Add additional word with the operand data in the 
             next address after command word */
            fAddImmediateWord(fileP, x->op2->data, (x->memAddress)+wordNum);
            break;
        case DIRECT:
            fAddDirectWord(fileP, x->op2->data, x->memAddress+wordNum);
            break;
        case REGISTER:
            fAddRegisterWord(fileP, NULL, x->op2->data, x->memAddress+wordNum);
            break;
	case STRUCT_ADD:
            n = strlen(x->op2->data);
            (x->op2->data)[n-2] = '\0'; /* Ignore struct member */
            fAddDirectWord(fileP, x->op2->data, x->memAddress+wordNum);
            wordNum++;  /* Increment word number - struct address is 2 additional words */
            
            (x->op2->data)[n-2] = '.'; /* Retrive struct member data */
            fAddImmediateWord(fileP, atoi((x->op1->data)+(n-1)), x->memAddress+wordNum);
                /* +(n-1) to skip to struct member,
                 * addressOffset+2 - this is the 2nd additional word */
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
    addressCode = x.memAddress;  /* Command code's address is the first in in instruction */
    
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
    
    /* No need to update coding type - always ABSOLUTE for commands */
    
    /* Convert to string representation */
    intToBase32STR(addressCode, addressSTR);
    intToBase32STR(wordCode, wordSTR);
    
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
    intToBase32STR(address, addressSTR);
    intToBase32STR((atoi(data)<<CODE_DATA_LOC), wordSTR);
    
    /* No need to change coding type for immediate */
    
    /* Add new line to fileP */
    fAddLine(fileP, addressSTR, wordSTR);
}

/* 
 * fAddRegisterWord: Add register word with given address to fileP
 */
void fAddRegisterWord(FILE *fileP, char *srcData, char *destData, int address) {
    int wordData;
    char addressSTR[BASE_32_LEN+1]; /* +1 for '\0' */
    char wordSTR[BASE_32_LEN+1];
    
    /***** Add registers number to word's data *****/
    wordData = 0;
    
    /* Add src register number if exist */
    if(srcData)
        wordData |= atoi(srcData+1)<<CODE_SRC_OP_LOC; /* +1 to skip 'r' register prefix */
    
    /* Add dest register number if exist */
    if(destData)
        wordData |= atoi(destData+1)<<CODE_DEST_OP_LOC;
    
    /* Convert to string representation */
    intToBase32STR(address, addressSTR);
    intToBase32STR(wordData, wordSTR); 
    
    /* No need to change coding type for register - always ABSOLUTE */
    
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
    /* No need to check is exist, check done in second pass */
    
    /* Get word data */
    wordCode = sym->address<<CODE_DATA_LOC;
    
    /* Get word coding type */
    wordCode |= (sym->address == EXTERNAL_ADDRESS)?EXTERNAL:RELOCATABLE;
    
    /* Convert to string representation */
    intToBase32STR(address, addressSTR);
    intToBase32STR(wordCode, wordSTR);
    
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

/* createExtEntFiles:   create extern and entry file of current active
                        assembly file (if needed) */
void createExtEntFiles() {
    FILE *ext, *ent;
    int i;
    
    /* if there are external symbols create file */
    if (externExist)
        ext = extFileOpen();
    
    /* if there are entries symbols create file */
    if (entryExist)
        ent = entFileOpen();

    /* Scan symbols table and add externals or entries to files when occure */
    for (i=0; i<HASHSIZE; i++) {
        if(symTable[i])
            fAddExtEnt(symTable[i], ext, ent);
    }
    
    /* File close if need */
    if (externExist)
        fclose(ext);
    if (entryExist)
        fclose(ent);
}

/* fAddExtEnt:  Scan symbols table and add externals to ext or entries to 
 *              ent when occure */
void fAddExtEnt(symbol *sym, FILE *ext, FILE *ent) {
    char addressSTR[BASE_32_LEN+1]; /* +1 for '\0' */
    char wordSTR[BASE_32_LEN+1];
    
    if (sym->isEntry) {                             /* Add to ent if entry */
        intToBase32STR(sym->address, addressSTR);
        fAddLine(ent, sym->name, addressSTR);
    } else if (sym->address == EXTERNAL_ADDRESS) {  /* Add to ext if external */
        intToBase32STR(sym->address, addressSTR);
        fAddLine(ext, sym->name, addressSTR);
    }
    
    /* Recursive call to continue table scan */
    if (sym->next)
        fAddExtEnt(sym->next, ext, ent);
}

/* extFileOpen: Opens new file for ext output file.
                Delete if exist */
FILE *extFileOpen() {
    char *extFileName;
    FILE *ext;
    
    /* Determine extern file name */
    extFileName = strdcat(fileName, externFileExtension);
    
    /* Open externals file */
    ext = fopen(extFileName, "w+");
    if (!ext) { /* If fopen fails print error and stop process */
        fprintf(stderr, "Error: cannot create file %s: %s\n", extFileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    free(extFileName);
    return ext;
}

/* entFileOpen: Opens new file for ent output file.
                Delete if exist */
FILE *entFileOpen() {
    char *entFileName;
    FILE *ent;
    
    /* Determine extern file name */
    entFileName = strdcat(fileName, entryFileExtension);
    
    /* Open object file */
    ent = fopen(entFileName, "w+");
    if (!ent) { /* If fopen fails print error and stop process */
        fprintf(stderr, "Error: cannot create file %s: %s\n", entFileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    free(entFileName);
    return ent;
}