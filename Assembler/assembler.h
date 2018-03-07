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
#define EXTERNAL_ADDRESS 0             /* Empty address and default for externs*/
#define FALSE	0                   /* Return value is false or invalid */
#define TRUE	1                   /* Return value is true or valid */
#define CPU_COMMANDS_NUM 16         /* Number of CPU command available */
#define MEMORY_START_ADDRESS 100    /* First address in memory to use */
#define OB_SUFFIX_LEN 3             /* Length of suffix for object file include '.' */
#define ENT_SUFFIX_LEN 4            /* Length of suffix for entry file include '.' */
#define EXT_SUFFIX_LEN 4            /* Length of suffix for externals file include '.' */
#define BASE_32_LEN 2               /* Length of 10-bit number in Strange 32 Base */
#define BITS_MASK_10BITS 1023       /* Mask to ignore higher bits than first 10 */
#define BITS_MASK_5BITS 31          /* Mask to ignore higher bits than first 5 */

#define CODE_OPCODE_LOC 6           /* Start bit of opcode location in coded instruction */
#define CODE_SRC_OP_LOC 4           /* Start bit of source addressing type location in coded instruction */
#define CODE_DEST_OP_LOC 2          /* Start bit of destination addressing type location in coded instruction */
#define CODE_DATA_LOC 2             /* Start bit of data location in additional words */


/******************** Main structure global variables ***********************/
#ifndef MAIN_C
instruction instArr[MAX_INSTRUCTIONS]; 		/* Instructions array */
extern int instIndex;				/* Instructions array index */
extern int dataArr[MAX_INSTRUCTIONS]; 		/* Data Array */
extern int IC; 					/* Instructions counter */
extern int DC; 					/* Data Counter */
extern int errorsDetected; 			/* Errors flag */
extern int entryExist;                          /* Entry existance flag */
extern int externExist;                         /* Extern existance flag */
extern FILE *fp; 				/* Active file pointer */
extern char *fileName; 				/* Name of active file */
#endif

/***************** Constants in constants.c declerations ********************/
#ifndef CONSTANTS_C 
extern const char const *errorsTab[];           /* Errors constants table */
extern const operatorNode CPUCommands[];        /* Commands constants table */
extern const char const *keywordTab[];          /* Keywords table */
extern const char const base32Digit[];          /* Strange Base-32 digits */
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
        REGISTER = 8, EMPTY = 16
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

/* Code words coding types:
 *      ABSOLUTE        Address won't change after program load 
 *      EXTERNAL        Data is depend on external information
 *      RELOCATABLE     Address depend on memory address received while 
 *                      program loaded
*/ 
enum codingType {ABSOLUTE, EXTERNAL, RELOCATABLE};

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
        int memAddress;                     /* Address in memory */
} instruction;

/* Holds line information to pass between functions */
typedef struct {
	char data[MAX_LINE_LEN];            /* Contains actual line */
	int lineNum;                        /* Line number in file */
	int i;                              /* Line index */
} line;


#endif
