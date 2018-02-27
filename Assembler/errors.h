
enum errorsShort {
	ERR_COMMA_BEFOR_PARAM,				/* Invalid comma before parameter */
	ERR_COMMA_EXPECTED,						/* Expected comma */
	ERR_MEMORY,										/* Not enough memory. Exiting... */
	ERR_LABEL_MISSING,						/* Label name missing */
	ERR_LABEL_LONG,								/* Invalid label: length exceeded maximum allowed */
	ERR_LABEL_KEYWORD,						/* Invalid label: cannot be a keyword */
	ERR_LABEL_EMPTY_LINE,					/* Invalid label: label in empty line */
	ERR_LABEL_LETTER,							/* Invalid label: must start with letter */
	ERR_LABEL_CHAR,								/* Invalid label: contains non-alphanumeric characters */
	ERR_LABEL_ENTRY,							/* Invalid label: assigned to entry instruction */
	ERR_PARAM_SIGN,								/* Invalid parameter: received sign without number */
	ERR_PARAM_CHAR,								/* Invalid parameter: received invalid character */
	ERR_PARAM_BIG,								/* Invalid parameter: number is too big and not supported by the system */
	ERR_LINE_LEN,									/* Line length is not supported */
	ERR_UNKNOWN_CMD,							/* Unknown command */
	NON_ERROR
};
