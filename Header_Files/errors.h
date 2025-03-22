/* errors.h */
#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
    /* General errors */
    ERROR_SUCCESS,
    ERROR_MEMORY_ALLOCATION,
    ERROR_NULL_POINTER,
    ERROR_FILE_PROCESSING,
    ERROR_ASSEMBLY_FAILED,
    ERROR_FILE_DELETE,
    ERROR_VPC_STORAGE_FULL,

    /* File errors */
    ERROR_MISSING_AS_FILE,
    ERROR_FILENAME_TOO_LONG,
    ERROR_FILE_NOT_EXIST,
    ERROR_LINE_TOO_LONG,
    ERROR_FILE_WRITE,
    ERROR_FILE_READ,
    ERROR_OBJECT_FILE_CREATE,
    ERROR_ENTRY_FILE_CREATE,
    ERROR_EXTERNAL_FILE_CREATE,

    /* Mcro errors */
    ERROR_MCRO_NO_NAME,
    ERROR_MCRO_DUPLICATE,
    ERROR_MCRO_BEFORE_DEF,
    ERROR_MCRO_TOO_LONG,
    ERROR_MCRO_IS_REGISTER,
    ERROR_MCRO_ILLEGAL_CHAR,
    ERROR_MCRO_ILLEGAL_START,
    ERROR_MCRO_RESERVED_NAME,
    ERROR_MCRO_ILLEGAL_NAME,
    ERROR_MCRO_UNEXPECTED_TEXT,
    ERROR_MACRO_CALL_EXTRA_TEXT,

    /* Entry/Extern/Label errors */
    ERROR_ENTRY_MISSING_LABEL,
    ERROR_EXTERN_MISSING_LABEL,
    ERROR_ENTRY_INSTEAD_OF_EXTERN,
    ERROR_ENTRY_EXTRA_TEXT,
    ERROR_EXTERN_EXTRA_TEXT,
    ERROR_LABEL_TOO_LONG,
    ERROR_ILLEGAL_LABEL_START,
    ERROR_ILLEGAL_LABEL_CHAR,
    ERROR_LABEL_IS_RESERVED_WORD,
    ERROR_ILLEGAL_LABEL,
    ERROR_LABEL_DUPLICATE,
    ERROR_LABEL_IS_MCRO_NAME,
    ERROR_UNDEFINED_LABEL,
    ERROR_UNDEFINED_ENTRY_LABEL,
    ERROR_UNDEFINED_LABEL_RELATIVE,
    ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL,
    ERROR_RELATIVE_ADDRESSING_TO_DATA,
    ERROR_LABEL_USED_IN_SAME_LINE,
    ERROR_LABEL_NOT_DEFINED_IN_FILE,
    ERROR_LABEL_IS_REGISTER,
    ERROR_EXTERN_LABEL_CONFLICT,
    ERROR_LABEL_ALREADY_EXTERN,
    ERROR_DUPLICATE_ENTRY_LABEL,
    ERROR_MAYBE_MEANT_ENTRY,
    ERROR_MAYBE_MEANT_EXTERN,

    /* Command errors */
    ERROR_UNKNOWN_COMMAND,
    ERROR_INVALID_PARAM_COUNT,
    ERROR_MISSING_COMMA,
    ERROR_CONSECUTIVE_COMMAS,
    ERROR_EXTRA_TEXT_AFTER_COMMAND,
    ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND,
    ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND,
    ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND,
    ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND,
    ERROR_INVALID_DIRECT_FIRST_OPERAND,
    ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND,
    ERROR_INVALID_IMMEDIATE_OPERAND,
    ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND,
    ERROR_INVALID_REGISTER_FIRST_OPERAND,
    ERROR_INVALID_REGISTER_SECOND_OPERAND,
    ERROR_INVALID_DATA_REAL_NUMBER,
    ERROR_INVALID_DATA_MISSING_COMMA,
    ERROR_NOT_EXTERN_LINE,
    ERROR_NOT_ENTRY_LINE,

    /* Data/Storage/String errors */
    ERROR_INVALID_DATA_NO_NUMBER,
    ERROR_INVALID_DATA_NON_NUMERIC,
    ERROR_INVALID_DATA_TRAILING_COMMA,
    ERROR_INVALID_DATA_UNEXPECTED_CHAR,
    ERROR_INVALID_STRING_NO_QUOTE,
    ERROR_INVALID_STRING_MISSING_END_QUOTE,
    ERROR_INVALID_STRING_EXTRA_CHARS,
    ERROR_INVALID_STORAGE_DIRECTIVE,
    ERROR_INVALID_LABEL_CONTENT,
    ERROR_INVALID_DATA_TOO_LARGE,
    ERROR_STRING_NO_VALUE
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
void print_warning_no_line(WarningCode code);
const char* get_error_message(ErrorCode code);
const char* get_error_name(ErrorCode code);
const char* get_warning_name(WarningCode code);
const char* get_warning_message(WarningCode code);

#endif /* ERRORS_H */