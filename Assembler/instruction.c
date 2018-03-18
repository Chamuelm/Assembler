/* 
 * File:    instruction.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains instruction, operand and operator treatment
 * 
 */
#include "include/assembler.h"
#include <stdlib.h>
#include <string.h>


/********************** Functions Definitions *************************/

/* addinstruction: Add instruction node to instructions array
 * and calculate number of actual assembly instructions and adds to
 * instructions counter IC
 */
void addInstruction(operatorNode *c, operand *operand1, operand *operand2, int lineNumber) {
    instArr[instIndex].cmd = c;
    instArr[instIndex].op1 = operand1;
    instArr[instIndex].op2 = operand2;
    instArr[instIndex].lineNum = lineNumber;
    instArr[instIndex].memAddress = MEMORY_START_ADDRESS+IC;
    
    IC += calcInstructions(&instArr[instIndex]);	/* Instructions counter increment */
    instIndex++;					/* Instructions array index increment */
}

/* freeOperand: Release dynamically allocated operand */
void freeOperand(operand *op){
    free(op->data);
    free(op);
}

/* freeInstArr: Release dynamically allocated operands of instructions in instArr */
void freeInstArr() {
    int i;
    
    for(i=0; i< instIndex; i++) {
        if (instArr[i].op1)
            freeOperand(instArr[i].op1);
        if (instArr[i].op2)
            freeOperand(instArr[i].op2);
    }
}


/* calcInstructions:	Calculate number of instructions
 * based on operators left in line
 */
int calcInstructions(instruction *inst) {
    enum addressingType op1Type, op2Type;
    int i = 1;
    
    if (inst->op1)
        op1Type = inst->op1->type;
    if (inst->op2)
        op2Type = inst->op2->type;
    
    /*
     * Increment i by type of addressing type:
     *	All but struct required 1 additional word
     *	Struct required 2 additional words.
     *	If both are REGISTER they can share the same word
     */
    if (inst->op1) {
        switch(op1Type) {
            case EMPTY:
                break;
            case REGISTER:
            case IMMEDIATE:
            case DIRECT:
                i++;
                break;
            case STRUCT_ADD:
                i += 2;
        }
    }
    
    if (inst->op2) {
        switch(op2Type) {
            case EMPTY:
                break;
            case REGISTER:
                if (op1Type != REGISTER)
                    i++;	/* If op1Type is REGISTER it use */
                break;	/* the same additional word */
            case IMMEDIATE:
            case DIRECT:
                i++;
                break;
            case STRUCT_ADD:
                i += 2;
        }
    }
    
    return i;
}
