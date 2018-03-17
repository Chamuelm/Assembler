/* 
 * File:    main.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Main drive function and related functions.
 * 
 * Assembler program explanation:
 * For each input file:
 *  1. First pass includes instruction nodes creation, error checks,
 *      symbol table creation.
 *  2. Second pass includes variable existence checks and creating externals table
 *  3. If no errors - output files creation
 * 
 */

#define MAIN_C  /* Include protector for extern declaration in assembler.h */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "include/assembler.h"



/************************ Global Variables ***************************/
/*instruction *instArr = (instruction *)malloc(sizeof(instruction)*MAX_INSTRUCTIONS); */
instruction instArr[MAX_INSTRUCTIONS];  /* Instructions array */ 
int instIndex;                          /* Instructions array index */
int *dataArr;          /* Data Array */
int IC;                                 /* Instructions counter */
int DC;                                 /* Data Counter */
int errorsDetected;                     /* Errors flag */
int entryExist;                         /* Entry existence flag */
int externExist;                        /* Extern existence flag */
FILE *fp;                               /* Active file pointer */ 
char *fileName;                         /* Name of active file */

/***************** Internal Functions Declarations ********************/
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
    
    symTableInit();    
    extTableInit();
    
    while (--argc > 0) { /* Processing for each input file */
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
    /* Variables initialization */
    IC = DC = 0;	/* Counter initialization */
    instIndex = 0;      /* instructions' array index initialize */
    entryExist = 0;     /* Entry existence flag initialization */
    externExist = 0;    /* Extern existence flag initialization */
    errorsDetected = 0; /* Errors flag initialization */
    
    dataArr = (int *)malloc(sizeof(int)*MAX_INSTRUCTIONS);
    if (!dataArr)
        exitMemory();
    
    /* Main functions calls */
    firstPass();
    secondPass();
    
    /* Exit if errors deceted, otherwise create output files */
    if (errorsDetected) {
    	fprintf(stderr, "File %s is not assembled due to the errors above.\n", fileName);
    	return;
    } else {
    	createOutputFiles();
                printf("File %s sucessfully assembled!\n", fileName);
    }

   /* Memory release */
    symTableRelease();
    extTableRelease();
    freeInstArr();
    free(dataArr);
}


/* lerror:	Print error message to stderr and increase error counter */
void lerror(error err, int lineNum) {
    fprintf(stderr, "Error, line %d: %s.\n", lineNum, errorsTab[err]);
    errorsDetected++;
}

/* lerror:	Like lerror but with 2 string parameters */
void lerror2s(error err, char *s, int lineNum) {
    fprintf(stderr, "Error, line %d: %s: %s.\n", lineNum, errorsTab[err], s);
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

