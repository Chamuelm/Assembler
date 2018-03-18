/* 
 * File:    firstPass.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains external function declarations of firstPass.c
 * 
 */

#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include "line.h"

void firstPass();
enum errorsShort checkCommandAddType(operatorNode *c, operand *op1, operand *op2);

#endif /* FIRSTPASS_H */

