#ifndef ASSEMBLER_H
#define ASSEMBLER_H
/************************* Constants definitions ****************************/
#define MAX_LINE_LEN 81							/* Maximum length of line in code includes '\n' */
#define MAX_INSTRUCTIONS 1000				/* Maximum number of instructions in one file */
#define MAX_DATA  1000		   				/* Maximum number of data in one file */
#define MAX_LABEL_LEN 30						/* Maximum length of label */
#define MAX_NUMBER	512							/* Maximum number allowed in computer' hardware */
#define MIN_MUNBER	-512						/* Minimum number allowed in computer' hardware */
#define SIGN_BIT_MASK	1024					/* Bit mask of sign bit in 10-bit number */
#define EMPTY_ADDRESS 0							/* Empty address */
#define FALSE_RETURN -1							/* Return value is false or invalid */
#define TRUE_RETURN		1							/* Return value is true or valid */
#define CPU_COMMANDS_NUM 16					/* Number of CPU command available */


/******************** Main structure global variables ***********************/
#ifndef MAIN_H
extern int instArr[MAX_INSTRUCTIONS]; 					/* Instructions array */
extern int dataArr[MAX_INSTRUCTIONS]; 					/* Data Array */
extern int IC; 																	/* Instructions counter */
extern int DC; 																	/* Data Counter */
extern int errorsDetected; 											/* Errors flag */
extern FILE *fp; 																/* Active file pointer */
extern char *fileName; 													/* Active file name */
#endif

/********************	 	Symbol table file include		 ***********************/
#include "symbol.h"
#include "errors.h"
extern const char const *errorsTab[];	/* Errors constants table */



/*
 * List of instruction types used to indicate the type of current
 * instruction in instructions table:
 * 0	COMMAND				Contains basic command data such as CPU operation,
 * 							source and destination addressing type and encoding type
 * 1	NUM					Contains source and destination register numbers and encoding type
 * 2 	LABEL_ADDRESS		Contains label address (8-bit) and encoding type
 * 3	NUMBER				Contains number (8-bit) and encoding type
 * 4	STRUCT_ITEM_NUM		Contains integer filled with 1 or 2 for desired struct field
 *
 *
enum instructionType {
	COMMAND, REG_NUM, LABEL_ADDRESS, NUMBER, STRUCT_ITEM_NUM
};
*/

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
	IMMEDIATE = 1, DIRECT = 2, STRUCT_ADD = 4, REGISTER = 8, EMPTY = 16
};

/* Allowed addressing types to use in commands struct */ 
enum allowedAddTypes {
	NONE 			= 0;
	ALL_ADD		=	IMMEDIATE|DIRECT|STRUCT_ADD|REGISTER;
	LABEL_ADD = DIRECT|STRUCT_ADD;
	REG_ADD		=	DIRECT|STRUCT_ADD|REGISTER;
};

/* Types of lines in source code */
enum lineTypes {
	COMMAND, DATA, STRING, STRUCT, ENTRY, EXTERN, UNKNOWN
};

/* Contains instructions operand data */
typedef struct {
	char data[MAX_LINE_LEN];
	enum addressingType type;
} operand;

/* CPU command struct */
typedef struct {
	char *name;											/* Command name */
	unsigned int opcode : 4;				/* Opcode */
	unsigned int operatorsNum : 2;	/* Number of operators */
	unsigned int allowedSrc : 5;		/* Allowed source addresing type */
	unsigned int allowedDest : 5;		/* Allowed destination addresing type */
} command;



typedef struct {
	char data[MAX_LINE_LEN];				/* Contains actual line */
	unsigned int flags : 6;					/* Flags as described below */
	int lineNum;										/* Line number in file */
	int i;													/* Line index */
	int symbolAddress;							/* Holds address if symbol */
	enum lineTypes lineType;				/* Line type */
	char symbolName[MAX_LABEL_LEN];	/* Symbol name if exist */
} line;


/* Flags for use in line read functions */
#define COMMA					1			/* Received comma at end of word */
#define EOL						2			/* Received '\n' at end of word */
#define VALID_SYM			4			/* Symbol is valid */
#define SYMBOL				8			/* Line have symbol */
#define LINE_TOO_LONG	16		/* Line is too long */

/* Holds instruction entry for instructions table
 typedef struct{
 int type;									 Holds current instruction type
 int encodingType;							 Encoding type

 Instruction type is COMMAND:
 int operation;								 Operation name
 int srcAddType;								 Source operand addressing type
 int destAddType;							 Destination operand addressing type

 Instruction type is REG_NUM
 int srcReg;                                  Source registry number (1-8)
 int destReg;								 Destination registry number (1-8)

 For use by instruction type:
 * COMMAND 				not in use
 * REG_NUM				not in use
 * LABEL_ADDRESS		contains the label address
 * NUMBER				contains the number
 * STRUCT_ITEM_NUM	 	contains the item number (1 or 2)

 int val;								 Number in direct addressing

 }instNode;

 Holds data entry for data table
 typedef struct{
 int data;   								Data entry
 int encodingType;							Encoding type
 }dataNode;*/

/* #### Function declerations #### */
/* ########### main.c ########### */
void assembler(char *fileName, FILE *fp);

#endif
