#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* firstPass: receive file pointer to process and pointers for
 * data storage as described.*/
int firstPass(FILE *fp,
						int *instC,							/* Instructions counter for return */
						int *dataC,							/* Data counter for return */
						int *instArr,						/* Instructions array */
						label *labelsArr,				/* Labels array */
						char **entriesArr)			/* Entries array */
{
	char word[MAX_LINE_LEN], symbolName[MAX_LINE_LEN];
	char line[MAX_LINE_LEN];									/* Line buffer */
	int buffIndex;														/* Line index */
	int IC = 0, DC = 0;				/* Instructions and data counters
	 	 	 	 	 	 	 	 	 	 	 	 	 	 * Easy to use it then received pointers */
	int lineC = 0;						/* Line counter */
	int isSymbol;

	/* Proccessing each line */
	while(line = fgets(line, MAX_LINE_LEN, fp))
	{
		lineC++;
		isSymbol = 0;
		if (line[0] == '\0' || line[0] == '\n')		/* If empty line */
			continue;

		if(sscanf(line, "%s", word))		/* Reads first word in line */
		{
			if (word[strlen(word)-1] == ':')
			{
				strcpy(symbolName, word);
				if (checkSymbol(symbolName))			/* Check if valid name and not exist */
					isSymbol = 1;

				/* Receive next word in command if symbol */
				if (!sscanf(line, "%s", word))
				{					/* Empty line after label */
					lerror("Empty label", lineC);
					continue;
				}
			}

		}
	}
}
