/* 
 * File:    main.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Main drive function and related functions.
 * 
 * Assembler program explanation:
 * For each input file:
 *  1. First pass includes instruction nodes creation, name and error checks,
 *      symbol table creation.
 *  2. Second pass includes memory adresses updates in symbols table and 
 *      check if all used symbols are truly exist.
 *  3. If no errors - output files creation
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#define MAIN_C
#include "assembler.h"

instruction instArr[MAX_INSTRUCTIONS];  /* Instructions array */
int instIndex;                          /* Instructions array index */
int dataArr[MAX_INSTRUCTIONS];          /* Data Array */
int IC;                                 /* Instructions counter */
int DC;                                 /* Data Counter */
int errorsDetected;                     /* Errors flag */
FILE *fp;                               /* Active file pointer */ 
char *fileName;                         /* Name of active file */

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Error: No input files entered.\n");
        printf("Usage: assembler file1 file2 ...\n");
        exit (EXIT_FAILURE);
    }
    
    while (--argc > 0) { /* Proccesing for each input file */
        fileName = ++argv;
        fp = fopen(fileName, "r");
        if (fp) { /* Check for valid file open */
            errorsDetected = 0;
            assembler();
            fclose(fp);
        } else
            fprintf(stderr, "Error: file %s was not compiled: %s\n", fileName, strerror(errno));
    }
    
    return EXIT_SUCCESS;
}

void assembler(char *fileName, FILE *fp) {
    IC = DC = instIndex = 0;				/* Counter initialization */
    symTableInit();			/* Symbol table initialization */
    
    firstPass();
    secondPass();
    
    if (errorsDetected()) {
        fprintf(stderr, "File %s is not compiled due to the errors above.\n", fileName);
        return;
    } else {
        createOutputFiles();
    }

    /* Memory release */
    symTableInit();
    freeInstArr();
}


/* lerror:	Print error message to stderr and increase error counter */
void lerror(char *s, int lineNum) {
    fprintf(stderr, "Error, line %d: %s.\n", lineNum, s);
    errorsDetected++;
}

/* lwarning: Print warning message to stderr */
void lwarning(char *s, int lineNum) {
    fprintf(stderr, "Warning, line %d: %s.\n", lineNum, s);
}


/* exitMemory:	exit the program if memory allocation was unsuccessful */
void exitMemory() {
    fprintf(stderr, "Not enough memory. Exiting...\n");
    exit(EXIT_FAILURE);
}

/* freeOperand: Release dynamically allocated operands of instructions in instArr */
void freeInstArr() {
    int i;
    
    for(i=0; i< instIndex; i++) {
        if (instArr[i].op1)
            freeOperand(instArr[i].op1);
        if (instArr[i].op2)
            freeOperand(instArr[i].op2);
    }
}

/* freeOperand: Release dynamically allocated operand */
void freeOperand(operand *op){
    free(op->data);
    free(op);
}