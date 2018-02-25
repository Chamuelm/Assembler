#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"





int main(int argc, char *argv[])
{
	char *fileName;
	FILE *fp;

	if (argc == 1)
	{
		perror("Error: No input files entered.\n");
		printf("Usage: assembler file1 file2 ...\n");
		exit(EXIT_FAILURE);
	}

	while(--argc > 0)								/* Proccesing for each input file */
	{
		fileName = ++argv;
		fp = fopen(fileName, "r");
		assembler(fileName, fp);
		fclose(fp);
	}
}

void assembler(char *fileName, FILE *fp)
{
	int instArr[MAX_INSTRUCTIONS];					/* Instructions array */
	label labelsArr[MAX_LABELS];							/* Labels array */
	char entries[MAX_LABELS][MAX_LABEL_LEN];	/* Entries array */
	int IC;																		/* Instructions counter */
	int DC;																		/* Data Counter */

	/* first pass */
	/* second pass */
	/* object file creation - need instructions array */
	/* extern symbol file creation - need symbols array */
	/* entry file creation - need labels array */
}

