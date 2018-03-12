/* 
 * File:    errors.h
 * Author:  Moshe Hamiel
 * ID:      308238716
 *
 * This enum contains error labels to use in lerror and lwarning functions.
 * The enums' values are used as index in error labels array in file constants.c
 * 
 */

#ifndef ERRORS_H
#define ERRORS_H

enum errorsShort {
    ERR_COMMA_BEFOR_PARAM,          /* Invalid comma before parameter */
    ERR_COMMA_EXPECTED,             /* Expected comma */
    ERR_COMMA_EXTRA,                /* Extra comma after parameter */
    ERR_MEMORY,                     /* Not enough memory. Exiting... */
    ERR_LABEL_MISSING,              /* Label name missing */
    ERR_LABEL_LONG,                 /* Invalid label: length exceeded maximum allowed */
    ERR_LABEL_KEYWORD,              /* Invalid label: cannot be a keyword */
    ERR_LABEL_EMPTY_LINE,           /* Invalid label: label in empty line */
    ERR_LABEL_LETTER,               /* Invalid label: must start with letter */
    ERR_LABEL_CHAR,                 /* Invalid label: contains non-alphanumeric characters */
    ERR_LABEL_ENTRY,                /* Invalid label: assigned to entry instruction */
    ERR_LABEL_EXTERN,               /* Invalid label: assigned to extern instruction */
    ERR_PARAM_SIGN,                 /* Invalid parameter: received sign without number */
    ERR_PARAM_CHAR,                 /* Invalid parameter: received invalid character */
    ERR_PARAM_BIG,                  /* Invalid parameter: number is too big and not supported by the system */
    ERR_PARAM_NAME,                 /* Invalid parameter: name is not valid */
    ERR_STRUCT_NAME,                /* Invalid parameter: struct name is not valid */
    ERR_MORE_PARAM0,                /* Extra parameters: Command may not receive parameters */
    ERR_MORE_PARAM1,                /* Extra parameters: Command may receive one parameter */
    ERR_MORE_PARAM2,                /* Extra parameters: Command may receive two parameters */
    ERR_MISS_PARAM,                 /* Missing parameter */
    ERR_PARAM_NO_NUM,               /* Invalid parameter: missing number */
    ERR_LINE_LEN,                   /* Line length is not supported */
    ERR_UNKNOWN_CMD,                /* Unknown command */
    ERR_CMD_EXTRA_TEXT,             /* Invalid text/parameters after command */
    ERR_STR_CLOSE,                  /* Expected \" (Quotation marks) in end of line */
    ERR_STR_MISS,                   /* Missing string parameter */
    ERR_STR_QMARK,                  /* Invalid string parameter, String must be in quotation marks */
    ERR_EXTERN_ENTRY,               /* Cannot mark external variable as entry */
    ERR_VAR_NOT_EXIST,              /* Variable is not exist */
    NON_ERROR
};

typedef enum errorsShort error;

#endif