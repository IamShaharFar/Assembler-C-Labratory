/* errors.h */
#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
    /* General errors: 0-9 */
    ERROR_SUCCESS = 0,
    ERROR_MEMORY_ALLOCATION = 1,

    /* File errors: 10-19 */
    ERROR_MISSING_AS_FILE = 10,
    ERROR_FILENAME_TOO_LONG = 11,
    ERROR_FILE_NOT_EXIST = 12,
    ERROR_MISSING_AM_FILE = 13,
    ERROR_LINE_TOO_LONG = 14,
    ERROR_FILE_WRITE = 15,
    ERROR_FILE_READ = 16,

    /* Mcro errors: 20-29 */
    ERROR_MCRO_NO_NAME = 20,
    ERROR_MCRO_EXTRA_TEXT = 21,
    ERROR_FILE_POSITION = 22,
    ERROR_ENDMACRO_EXTRA_TEXT = 23,
    ERROR_MCRO_DUPLICATE = 24,
    ERROR_MCRO_COPY_FAILED = 25,
    ERROR_MCRO_EXPANSION = 26,
    ERROR_MCRO_BEFORE_DEF = 27,
    ERROR_MCRO_ILLEGAL_NAME = 28,

    /* Syntax errors: 30-39 */
    ERROR_ILLEGAL_OPCODE = 30,
    ERROR_EXTRA_TEXT = 31,
    ERROR_ILLEGAL_ARGUMENT = 32,
    ERROR_MISSING_ARGUMENT = 33,
    ERROR_MISSING_COMMA = 34,
    ERROR_UNDEFINED_LABEL = 35,
    ERROR_ILLEGAL_ENTRY_LABEL = 36,
    ERROR_ILLEGAL_OPCODE_COMMA = 37,
    ERROR_EXCESS_COMMAS = 38,
    ERROR_MISPLACED_COMMA = 39,

    /* Label and register errors: 40-49 */
    ERROR_ILLEGAL_CHAR_OPCODE = 40,
    ERROR_ILLEGAL_CHAR = 41,
    ERROR_MISSING_NUMBER_COMMA = 42,
    ERROR_ILLEGAL_LABEL = 43,
    ERROR_MISSING_LABEL_COLON = 44,
    ERROR_ILLEGAL_REGISTER = 45,
    ERROR_COMMA_BEFORE_OPCODE = 46,

    /* Directive errors: 50-59 */
    ERROR_MISSING_DIRECTIVE_DOT = 50,
    ERROR_DATA_NON_NUMERIC = 51,
    ERROR_DATA_TRAILING_COMMA = 52,
    ERROR_STRING_MISSING_QUOTE = 53,
    ERROR_STRING_EXTRA_TEXT = 54,
    ERROR_IC_OVERFLOW = 55,
    ERROR_LABEL_DUPLICATE = 56,
    ERROR_EXTERN_CONFLICT = 57,
    ERROR_DATA_OUT_OF_RANGE = 58,
    ERROR_ILLEGAL_DIRECTIVE = 59
} ErrorCode;

typedef struct {
    ErrorCode code;
    const char* message;
} Error;

extern const Error errors[];

void print_error(ErrorCode code, int line_number);
void print_error_no_line(ErrorCode code);
const char* get_error_message(ErrorCode code);
const char* get_error_name(ErrorCode code);

#endif /* ERRORS_H */