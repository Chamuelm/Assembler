/*
 * output.c
 *
 *  Contains output files creation functions
 *      Author: Moshe hamiel
 *      ID:		308238716
 */
#include "assembler.h"
#include <stdio.h>

/* Create output files of active file */
void createOutputFiles() {
    /* object file creation - need instructions array */
    createObjectsFile();
    
    /* extern symbol file creation - need symbols array */
    /* entry file creation - need labels array */
}

void createObjectsFile() {
    
}

