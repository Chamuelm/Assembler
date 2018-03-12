/* 
 * File:    instruction.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Instruction, operand and operator struct definition and related functions decleration
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

/***************** External Functions Declerations ********************/
void addInstruction(operatorNode *c, operand *operand1, operand *operand2, int lineNumber);
void freeInstArr();
int checkOperandName(operand *op);
int calcInstructions(instruction *inst);

#endif /* INSTRUCTION_H */

