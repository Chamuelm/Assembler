/* 
 * File:    main.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains main.c functions declerations
 * 
 */

#ifndef MAIN_H
#define MAIN_H

#include "errors.h"

void lerror(error err, int lineNum);
void lerror2s(error err, char *s, int lineNum);
void lwarning(error err, int lineNum);
void exitMemory();


#endif