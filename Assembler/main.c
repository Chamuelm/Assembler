#include <stdio.h>
#include "assembler.h"

/* Line buffer and index */
char *currLine[MAX_LINE_LEN];
int buffIndex;

/* Labels array */
labelNode labels[MAX_LABELS];

/* Instructions array */
instNode insts[MAX_INSTRUCTIONS];
