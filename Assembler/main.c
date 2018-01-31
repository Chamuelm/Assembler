#include <stdio.h>
#include "assembler.h"

/* Line buffer and index */
char *currLine[MAX_LINE_LEN];
int buffIndex;

/* Labels array */
labelNode labels[MAX_LABELS];
/*int array */
/* Instructions array */
int instArr[MAX_INSTRUCTIONS];

int data[MAX_DATA_LEN];



