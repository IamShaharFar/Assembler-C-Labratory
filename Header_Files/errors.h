/* errors.h */
#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
    /* General errors: 0-9 */
    ERROR_SUCCESS = 0,
    ERROR_MEMORY_ALLOCATION = 1,
    ERROR_NULL_POINTER = 2,
    ERROR_FILE_PROCESSING = 3,
    ERROR_ASSEMBLY_FAILED = 4,
    ERROR_FILE_DELETE = 5,
    ERROR_VPC_STORAGE_FULL = 6,

    /* File errors: 10-19 */
    ERROR_MISSING_AS_FILE = 10,
    ERROR_FILENAME_TOO_LONG = 11,
    ERROR_FILE_NOT_EXIST = 12,
    ERROR_LINE_TOO_LONG = 14,
    ERROR_FILE_WRITE = 15,
    ERROR_FILE_READ = 16,

    /* Mcro errors: 20-29 */
    ERROR_MCRO_NO_NAME = 20,
    ERROR_MCRO_DUPLICATE = 21,
    ERROR_MCRO_BEFORE_DEF = 22,
    ERROR_MCRO_TOO_LONG = 23,
    ERROR_MCRO_IS_REGISTER = 24,
    ERROR_MCRO_ILLEGAL_CHAR = 25,
    ERROR_MCRO_ILLEGAL_START = 26,
    ERROR_MCRO_RESERVED_NAME = 27,
    ERROR_MCRO_ILLEGAL_NAME = 28,
    ERROR_MCRO_UNEXPECTED_TEXT = 29,

    /* Label and register errors: 40-49 */
    ERROR_ENTRY_MISSING_LABEL = 34,
    ERROR_EXTERN_MISSING_LABEL = 35,
    ERROR_ENTRY_INSTEAD_OF_EXTERN = 36,
    ERROR_ENTRY_EXTRA_TEXT = 37,
    ERROR_EXTERN_EXTRA_TEXT = 38,
    ERROR_LABEL_TOO_LONG = 39,
    ERROR_ILLEGAL_LABEL_START = 40,
    ERROR_ILLEGAL_LABEL_CHAR = 41,
    ERROR_LABEL_IS_RESERVED_WORD = 42,
    ERROR_ILLEGAL_LABEL = 43,
    ERROR_LABEL_DUPLICATE = 44,
    ERROR_LABEL_IS_MCRO_NAME = 45,
    ERROR_UNDEFINED_LABEL = 46,
    ERROR_UNDEFINED_LABEL_RELATIVE = 47,
    ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL = 48,
    ERROR_LABEL_NOT_DEFINED_IN_FILE = 49,
    ERROR_LABEL_IS_REGISTER = 50,
    ERROR_EXTERN_LABEL_CONFLICT  = 67,
    ERROR_LABEL_ALREADY_EXTERN = 69,
    ERROR_DUPLICATE_ENTRY_LABEL = 79,
    ERROR_MAYBE_MEANT_ENTRY,
    ERROR_MAYBE_MEANT_EXTERN,

    /* Command errors: 50-59 */
    ERROR_UNKNOWN_COMMAND = 51,
    ERROR_INVALID_PARAM_COUNT = 52,
    ERROR_MISSING_COMMA = 53,
    ERROR_CONSECUTIVE_COMMAS = 54,
    ERROR_EXTRA_TEXT_AFTER_COMMAND = 55,
    ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND = 56,
    ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND = 57,
    ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND = 58,
    ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND = 59,
    ERROR_INVALID_DIRECT_FIRST_OPERAND = 60,
    ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND = 62,
    ERROR_INVALID_IMMEDIATE_OPERAND = 63,
    ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND = 64,
    ERROR_INVALID_REGISTER_FIRST_OPERAND,
    ERROR_INVALID_REGISTER_SECOND_OPERAND,
    ERROR_INVALID_DATA_REAL_NUMBER = 65,
    ERROR_INVALID_DATA_MISSING_COMMA = 66,
    ERROR_NOT_EXTERN_LINE = 68,

    /* Data storage errors */
    ERROR_INVALID_DATA_NO_NUMBER = 70,
    ERROR_INVALID_DATA_NON_NUMERIC = 71,
    ERROR_INVALID_DATA_TRAILING_COMMA = 72,
    ERROR_INVALID_DATA_UNEXPECTED_CHAR = 73,
    ERROR_INVALID_STRING_NO_QUOTE = 74,
    ERROR_INVALID_STRING_MISSING_END_QUOTE = 75,
    ERROR_INVALID_STRING_EXTRA_CHARS = 76,
    ERROR_INVALID_STORAGE_DIRECTIVE = 77,
    ERROR_INVALID_LABEL_CONTENT = 78,
    ERROR_INVALID_DATA_TOO_LARGE = 81, 
    ERROR_STRING_NO_VALUE = 80
} ErrorCode;

typedef enum {
    /* General warnings */
    WARNING_LABEL_BEFORE_EXTERN = 1,  /* Warns about ignored label before .extern */
    WARNING_LABEL_BEFORE_ENTRY = 2,
    WARNING_UNUSED_LABEL,             
    WARNING_MULTIPLE_SPACES,          
    WARNING_REDUNDANT_ENTRY          /* Warns if .entry is declared multiple times */
} WarningCode;

typedef struct {
    ErrorCode code;
    const char* name;     
    const char* message; 
} Error;

typedef struct {
    WarningCode code;
    const char* name;
    const char* message;
} Warning;

extern const Error errors[];

extern const Warning warnings[];

void print_error_with_code(ErrorCode code, int line_number, const char *start, const char *end);
void print_error(ErrorCode code, int line_number);
void print_warning(WarningCode code, int line_number);
void print_error_no_line(ErrorCode code);
const char* get_error_message(ErrorCode code);
const char* get_error_name(ErrorCode code);
const char* get_warning_name(WarningCode code);
const char* get_warning_message(WarningCode code);

#endif /* ERRORS_H */