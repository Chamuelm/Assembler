/* 
 * File:    assembler.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * Main header. Contains constants definitions,
 * other headers inclusions, extern declerations.
 * 
 */


#ifndef ASSEMBLER_H
#define ASSEMBLER_H
/************************* Constants definitions ****************************/
#define MAX_LINE_LEN 81             /* Maximum length of line in code includes '\n' */
#define MAX_INSTRUCTIONS 1000       /* Maximum number of instructions in one file */
#define MAX_DATA  1000              /* Maximum number of data in one file */
#define MAX_LABEL_LEN 30            /* Maximum length of label */
#define MAX_NUMBER	512         /* Maximum number allowed in computer' hardware */
#define MIN_MUNBER	-512        /* Minimum number allowed in computer' hardware */
#define SIGN_BIT_MASK	1024        /* Bit mask of sign bit in 10-bit number */
#define EMPTY_ADDRESS 0             /* Empty address and default for externs*/
#define FALSE	0                   /* Return value is false or invalid */
#define TRUE	1                   /* Return value is true or valid */
#define CPU_COMMANDS_NUM 16         /* Number of CPU command available */
#define MEMORY_START_ADDRESS 100    /* First address in memory to use */


/******************** Main structure global variables ***********************/
#ifndef MAIN_C
instruction instArr[MAX_INSTRUCTIONS]; 		/* Instructions array */
extern int instIndex;				/* Instructions array index */
extern int dataArr[MAX_INSTRUCTIONS]; 		/* Data Array */
extern int IC; 					/* Instructions counter */
extern int DC; 					/* Data Counter */
extern int errorsDetected; 			/* Errors flag */
extern FILE *fp; 				/* Active file pointer */
extern char *fileName; 				/* Name of active file */
#endif

/***************** Constants in constants.c declerations ********************/
#ifndef CONSTANTS_C 
extern const char const *errorsTab[];           /* Errors constants table */
extern const operatorNode CPUCommands[];        /* Commands constants table */
extern const char const *keywordTab[];          /* Keywords table */
#endif

/*************************** Headers Inclutions ******************************/
#include "main.h"
#include "firstPass.h"
#include "secondPass.h"
#include "symbol.h"
#include "errors.h"
#include "utils.h"



/*************************** enums declerations ******************************/
/*
 * List of addressing types used to indicate the addressing type of
 * the current instruction.
 * addressing types:
 * 1 		IMMEDIATE				Immediate addressing
 * 2 		DIRECT					Direct addressing
 * 4 		STRUCT_ADD			Struct ‬‬
 * 8 		REGISTER	 			Register addressing
 * 16		EMPTY
 *
 */
enum addressingType {
	IMMEDIATE = 1, DIRECT = 2, STRUCT_ADD = 4,
        REGISTER = 8, EMPTY = 16, INVALID_ADD_TYPE = 32
};

/* Allowed addressing types to use in commands struct */ 
enum allowedAddTypes {
	NONE        =   0,
	ALL_ADD     =   IMMEDIATE|DIRECT|STRUCT_ADD|REGISTER,
	LABEL_ADD   =   DIRECT|STRUCT_ADD,
	REG_ADD     =   DIRECT|STRUCT_ADD|REGISTER
};

/* Types of lines in source code */
enum lineTypes {
	COMMAND, DATA, STRING, STRUCT, ENTRY, EXTERN, UNKNOWN
};

/************************** Structs declerations *****************************/


/* Contains instructions operand data */
typedef struct {
	char *data;
	enum addressingType type;
} operand;

/* CPU command struct */
typedef struct {
	char *name;                         /* Command name */
	unsigned int opcode : 4;            /* Opcode */
	unsigned int operatorsNum : 2;      /* Number of operators */
	unsigned int allowedSrc : 5;        /* Allowed source addresing type */
	unsigned int allowedDest : 5;       /* Allowed destination addresing type */
} operatorNode;

/* Holds instruction information for instructions table */
typedef struct {
	operatorNode *cmd;                  /* Command pointer */
	operand *op1, *op2;                 /* Operands of this instruction */
	int lineNum;                        /* Line number in source file */
} instruction;

/* Holds line information to pass between functions */
typedef struct {
	char data[MAX_LINE_LEN];            /* Contains actual line */
	int lineNum;                        /* Line number in file */
	int i;                              /* Line index */
} line;


#endif
