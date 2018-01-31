
#define MAX_LINE_LEN 81							/* Maximum length of line in code includes '\n' */
#define MAX_LABELS 256							/* Maximum number of labels in one file */
#define MAX_INSTRUCTIONS 256					/* Maximum number of instructions in one file */
#define MAX_LABEL_LEN 30						/* Maximum length of label */
#define MAX_DATA_LEN                            /* Maximum length of data */

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
 */
enum instructionType{COMMAND, REG_NUM, LABEL_ADDRESS, NUMBER, STRUCT_ITEM_NUM};


/*
 * List of addressing types used to indicate the addressing type of
 * the current instruction.
 * addressing types:
 * 0 	IMMEDIATE		immediate addressing
 * 1 	DIRECT			direct addressing
 * 2 	ACCESS_STRUCT	addressing access struct ‬‬
 * 3 	REG	direct 		register addressing
 *
 */
enum addressingType{IMMEDIATE, DIRECT, ACCESS_STRUCT, REG};


/*
 * List of CPU commands order by opcodes.
 */
enum CPUCommands{MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC,DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP};


/* Holds label entry for labels table */
typedef struct{
	char name[MAX_LABEL_LEN];					/* Label name */
	int address;								/* Address in memory */
	unsigned int isExternal : 1;				/* Is assigned to external variable */
	unsigned int isInst : 1;					/* Is assigned to instruction */
}labelNode;

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


