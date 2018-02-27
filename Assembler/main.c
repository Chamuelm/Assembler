#include <stdio.h>
#include <stdlib.h>
#define MAIN_H
#include "assembler.h"

int instArr[MAX_INSTRUCTIONS]; /* Instructions array */
int dataArr[MAX_INSTRUCTIONS]; /* Data Array */
int IC; /* Instructions counter */
int DC; /* Data Counter */
int errorsDetected; /* Errors flag */
FILE *fp;	/* Active file pointer */ 
char *fileName;	/*Active file name */

int main(int argc, char *argv[]) {


	if (argc == 1) {
		fprintf(stderr, "Error: No input files entered.\n");
		printf("Usage: assembler file1 file2 ...\n");
		exit (EXIT_FAILURE);
	}

	while (--argc > 0) /* Proccesing for each input file */
	{
		fileName = ++argv;
		fp = fopen(fileName, "r");
		if (fp) { /* Check for valid file open */
			errorDetected = 0;
			assembler();
			fclose(fp);
		}
		else
			fprintf(stderr, "Error: file %s was not compiled: %s\n", fileName, strerror(errnum));
	}
}

void assembler(char *fileName, FILE *fp) {
	IC = DC = 0;				/* Counter initalization */
	symTableInit();			/* Symbol table initializaion */
	if (!firstPass(fileName, fp)) {
		fprinf(stderr, "File %s is not compiled due to the errors above.\n", fileName);
		return;
	}
	

	/* first pass */
	/* second pass */
	/* object file creation - need instructions array */
	/* extern symbol file creation - need symbols array */
	/* entry file creation - need labels array */
}

