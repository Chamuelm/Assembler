/* 
 * File:    constants.c
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Contains constants for use in functions  
 * 
 */

#define CONSTANTS_C
#include <stdio.h>

/*
 * Constant array contain error messages to use in lerror and lwarning functions.
 */
const char const *errorsTab[] = {
    "Invalid comma before parameter",
    "Expected comma",
    "Extra comma after parameter",
    "Not enough memory. Exiting...",
    "Label name missing",
    "Invalid label: length exceeded maximum allowed",
    "Invalid label: cannot be a keyword",
    "Invalid label: label in empty line",
    "Invalid label: must start with letter",
    "Invalid label: contains non-alphanumeric characters",
    "Invalid label: assigned to entry instruction",
    "Invalid label: assigned to extern instruction",
    "Invalid parameter: received sign without number",
    "Invalid parameter: received invalid character",
    "Invalid parameter: number is too big and not supported",
    "Invalid parameter: name is not valid",
    "Invalid parameter: struct name is not valid",
    "Extra parameters: Command may not receive parameters",
    "Extra parameters: Command may receive one parameter",
    "Extra parameters: Command may receive two parameters",
    "Missing parameter",
    "Invalid parameter: missing number",
    "Line length is not supported",
    "Unknown command",
    "Invalid text/parameters after command",
    "Expected \" (Quotation marks) in end of line",
    "Missing string parameter",
    "Invalid string parameter, String must be in quotation marks",
    "Cannot mark external variable as entry",
    "Variable is not exist"
};

/* CPU commands:
 * 	Name		Name as string
 * 	Opcode		Opcode value
 * 	operators	Number of operators for this command
 * 	SrcAdd		Source addressing types allowed (Described in enum allowedAddTypes in assembler.h)
 * 	DestAdd		Destination addressing types allowed
 *  */
const operatorNode CPUCommands[] = {
/*  Name  	Opcode          Operators   SrcAdd          DestAdd	  */
    {"mov",	0,		2,          ALL_ADD,        REG_ADD},
    {"cmp",	1,		2,          ALL_ADD,        ALL_ADD},
    {"add",	2,		2,          ALL_ADD,        REG_ADD},
    {"sub",	3,		2,          ALL_ADD,        REG_ADD},
    {"not",	4,		1,          NONE,           REG_ADD},
    {"clr",	5,		1,          NONE,           REG_ADD},
    {"lea",	6,		2,          LABEL_ADD,      REG_ADD},
    {"inc",	7,		1,          NONE,           REG_ADD},
    {"dec", 	8,		1,          NONE,           REG_ADD},
    {"jmp", 	9,		1,          NONE,           REG_ADD},
    {"bne", 	10,		1,          NONE,           REG_ADD},
    {"red",	11,		1,          NONE,           REG_ADD},
    {"prn",	12,		1,          NONE,           ALL_ADD},
    {"jsr",	13,		1,          NONE,           REG_ADD},
    {"rts",	14,		0,          NONE,           NONE},
    {"stop",	15,		0,          NONE,           NONE}
};

/*
 * Constant array contain assembly keywords.
 * Not contains command names and register names
 * which are keywords too
 */
const char const *keywordTab[] = {
    ".data",
    ".struct",
    ".extern",
    ".string",
    ".entry",
    NULL
};