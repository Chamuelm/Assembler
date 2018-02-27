const char const *errorsTab[] = {
		"Invalid comma before parameter",
		"Expected comma",
		"Not enough memory. Exiting...",
		"Label name missing",
		"Invalid label: length exceeded maximum allowed",
		"Invalid label: cannot be a keyword",
		"Invalid label: label in empty line",
		"Invalid label: must start with letter",
		"Invalid label: contains non-alphanumeric characters",
		"Invalid label: assigned to entry instruction",
		"Invalid parameter: received sign without number",
		"Invalid parameter: received invalid character",
		"Invalid parameter: number is too big and not supported",
		"Line length is not supported",
		"Unknown command"
};

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
