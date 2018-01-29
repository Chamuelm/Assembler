
#define MAX_LINE_LEN 81
#define MAX_LABELS 256
#define MAX_INSTRUCTIONS 256

#define MAX_LABEL_LEN 30
typedef enum{
	command,regNum,address,number
}nType;
typedef struct{
	char name[MAX_LABEL_LEN];			/* Label name */
	int address;									/* Address in memory */
	unsigned int isExternal : 1;	/* Is assigned to external variable */
	unsigned int isInst : 1;			/* Is assigned to instruction */
} labelNode;

typedef struct{
	int opNum;										/* Operation number */
	int srcAddType;								/* Source operand addressing type */
	int destAddType;							/* Destination operand addressing type */
	int encodingType;							/* Encoding type */
	int srcReg;                                 /*Source registry number */
	int destReg;								/*Destination registry number*/
	int directNum;								/*Number in direct addressing*/
	int destAdd;								/*Destination Address*/
	int scrAdd;									/*Source Address*/
	int structAdd;								/*Struct address*/
	int structField;							/*Struct field number*/
} instNode;
