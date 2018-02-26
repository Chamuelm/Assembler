#ifndef MAX_LINE_LEN
/************************* Constants definitions ****************************/
#define MAX_LINE_LEN 81							/* Maximum length of line in code includes '\n' */
#define MAX_INSTRUCTIONS 1000				/* Maximum number of instructions in one file */
#define MAX_DATA  1000		   				/* Maximum number of data in one file */
#define MAX_LABEL_LEN 30						/* Maximum length of label */

#define FALSE_RETURN -1							/* Return value is false or invalid */
#define TRUE_RETURN		1							/* Return value is true or valid */


/******************** Main structure global variables ***********************/
#ifndef MAIN_DEF
extern int instArr[MAX_INSTRUCTIONS]; /* Instructions array */
extern int dataArr[MAX_INSTRUCTIONS]; /* Data Array */
extern label labelsArr[MAX_LABELS]; /* Labels array */
extern char entries[MAX_LABELS][MAX_LABEL_LEN]; /* Entries array */
extern int IC; /* Instructions counter */
extern int DC; /* Data Counter */
extern int LC; /* Labels counter */
extern int EC; /* Entries counter */
extern int errorDetected; /* Errors flag */
extern FILE *fp; /* Active file pointer */
extern char *fileName; /* Active file name */
#endif

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

/* Holds label entry for labels table */
typedef struct {
	char name[MAX_LABEL_LEN];		/* Label name */
	int address;								/* Address in memory */
	unsigned int isExternal :1; /* Is assigned to external variable */
	unsigned int isInst :1; 		/* Is assigned to instruction */
} label;

typedef struct {
	char data[MAX_LINE_LEN];		/* Contains actual line */
	unsigned int flags : 6;							/* Flags as described below */
	int lineNum;								/* Line number in file */
	int i;											/* Line index */
	int symbolAddress;					/* Holds address if symbol */
	enum lineTypes lineType;		/* Line type */
	int lastRead;								/* Length of last word read */
} line;

/* CPU commands ordered by opcodes. */
const command CPUCommands[] = {
/* 	Name  Opcode  Operators  SrcAdd	 		 DestAdd	  */
		{"mov",		0,		2,			ALL_ADD,		REG_ADD},
		{"cmp",		1,		2,			ALL_ADD,		ALL_ADD},
		{"add",		2,		2,			ALL_ADD,		REG_ADD},
		{"sub",		3,		2,			ALL_ADD,		REG_ADD},
		{"not",		4,		1,			NONE,				REG_ADD},
		{"clr",		5,		1,			NONE,				REG_ADD},
		{"lea",		6,		2,			LABEL_ADD,	REG_ADD},
		{"inc",		7,		1,			NONE,				REG_ADD},
		{"dec", 	8,		1,			NONE,				REG_ADD},
		{"jmp", 	9,		1,			NONE,				REG_ADD},
		{"bne", 	10,		1,			NONE,				REG_ADD},
		{"red",		11,		1,			NONE,				REG_ADD},
		{"prn",		12,		1,			NONE,				ALL_ADD},
		{"jsr",		13,		1,			NONE,				REG_ADD},
		{"rts",		14,		0,			NONE,				NONE},
		{"stop",	15,		0,			NONE,				NONE}
};

/* Flags for use in line read functions */
#define COMMA					1			/* Received comma at end of word */
#define EOL						2			/* Received '\n' at end of word */
#define VALID_SYM			4			/* Symbol is valid */
#define SYMBOL				8			/* Line have symbol */
#define INVALID_SYM		16		/* Symbol is invalid */
#define LINE_TOO_LONG	32		/* Line is too long */

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
