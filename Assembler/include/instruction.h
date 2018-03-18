/* 
 * File:    instruction.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Instruction, operand and operator struct definition and related functions declaration
 * 
 */

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "assembler.h"


/************************** Structs declerations *****************************/
/* Contains instructions operand data */
typedef struct operand_s {
    char *data;
    addrType type;
} operand;

/* CPU command struct */
typedef struct operatorNode_s {
    char *name;                         /* Command name */
    unsigned int opcode : 4;            /* Opcode */
    unsigned int operatorsNum : 2;      /* Number of operators */
    unsigned int allowedSrc : 5;        /* Allowed source addresing type */
    unsigned int allowedDest : 5;       /* Allowed destination addresing type */
} operatorNode;

/* Holds instruction information for instructions table */
typedef struct instruction_s {
    struct operatorNode_s *cmd;                  /* Command pointer */
    struct operand_s *op1, *op2;                 /* Operands of this instruction */
    int lineNum;                        /* Line number in source file */
    int memAddress;                     /* Address in memory */
} instruction;

/***************** External Functions Declarations ********************/
void addInstruction(operatorNode *c, operand *operand1, operand *operand2, int lineNumber);
void freeInstArr();
int calcInstructions(instruction *inst);
void freeOperand(operand *op);

#endif /* INSTRUCTION_H */

