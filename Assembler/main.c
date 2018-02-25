#include <stdio.h>
#include <stdlib.h>
#define MAIN_DEF
#include "assembler.h"

int instArr[MAX_INSTRUCTIONS]; /* Instructions array */
int dataArr[MAX_INSTRUCTIONS]; /* Data Array */
label labelsArr[MAX_LABELS]; /* Labels array */
char entries[MAX_LABELS][MAX_LABEL_LEN]; /* Entries array */
int IC; /* Instructions counter */
int DC; /* Data Counter */
int LC; /* Labels counter */
int EC; /* Entries counter */
int errorDetected; /* Errors flag */
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
			assembler(fileName, fp);
			fclose(fp);
		}
		else
			fprintf(stderr, "Error: file %s has not compiled: %s\n", fileName, strerror(errnum));
	}
}

void assembler(char *fileName, FILE *fp) {
	IC = DC = LC = EC = 0;
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

