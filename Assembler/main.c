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

#define MAIN_C  /* Include protector for extern decleration in assembler.h */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "include/assembler.h"



/************************ Global Variables ***************************/
/*instruction *instArr = (instruction *)malloc(sizeof(instruction)*MAX_INSTRUCTIONS); */
instruction instArr[MAX_INSTRUCTIONS];  /* Instructions array */ 
int instIndex;                          /* Instructions array index */
int dataArr[MAX_INSTRUCTIONS];          /* Data Array */
int IC;                                 /* Instructions counter */
int DC;                                 /* Data Counter */
int errorsDetected;                     /* Errors flag */
int entryExist;                         /* Entry existance flag */
int externExist;                        /* Extern existance flag */
FILE *fp;                               /* Active file pointer */ 
char *fileName;                         /* Name of active file */

/***************** Internal Functions Declerations ********************/
int main(int argc, char *argv[]);
void assembler();

/********************** Functions Definitions *************************/

int main(int argc, char *argv[]) {
    char *fullFileName;
    
    if (argc == 1) { /* No source files received */
        fprintf(stderr, "Error: No source files entered.\n");
        printf("Usage: assembler file1 file2 ...\n");
        exit (EXIT_FAILURE);
    }
    
    while (--argc > 0) { /* Proccesing for each input file */
        fileName = *++argv;
        /* Cat file extension */
        fullFileName = strdcat(fileName, sourceFileExtension);
        
        fp = fopen(fullFileName, "r");
        if (fp) { /* Check for valid file open */
            assembler();
            fclose(fp);
        } else
            fprintf(stderr, "Error: file %s was not compiled: %s\n", fileName, strerror(errno));
        
        free(fullFileName);
    }
    
    return EXIT_SUCCESS;
}

void assembler() {
    /* Varaiables initialization */
    IC = DC = 0;	/* Counter initialization */
    instIndex = 0;      /* instructions' array index initialize */
    entryExist = 0;     /* Entry existance flag initialization */
    externExist = 0;    /* Extern existance flag initialization */
    errorsDetected = 0; /* Errors flag initialization */
    symTableInit();     /* Symbol table initialization */
    
    /* Main functions calls */
    firstPass();
    secondPass();
    
    /* Exit if errors deceted, otherwise create output files */
    if (errorsDetected) {
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
void lerror(error err, int lineNum) {
    fprintf(stderr, "Error, line %d: %s.\n", lineNum, errorsTab[err]);
    errorsDetected++;
}

/* lwarning:    Print warning message to stderr */
void lwarning(error err, int lineNum) {
    fprintf(stderr, "Warning, line %d: %s.\n", lineNum, errorsTab[err]);
}


/* exitMemory:	Exit the program if memory allocation was unsuccessful */
void exitMemory() {
    fprintf(stderr, "Not enough memory. Exiting...\n");
    exit(EXIT_FAILURE);
}

