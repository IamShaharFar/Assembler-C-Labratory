/* errors.c */
#include <stdio.h>
#include "../Header_Files/errors.h"

const Error errors[] = {
    /* General errors */
    {ERROR_SUCCESS, "Operation completed successfully"},
    {ERROR_MEMORY_ALLOCATION, "Not enough memory available to complete operation"},

    /* File errors */
    {ERROR_MISSING_AS_FILE, "Missing source file - please provide a .as file"},
    {ERROR_FILENAME_TOO_LONG, "File name is too long - please use a shorter name"},
    {ERROR_FILE_NOT_EXIST, "Could not find the specified file"},
    {ERROR_MISSING_AM_FILE, "Could not find the .am file - run preprocessor first"},
    {ERROR_LINE_TOO_LONG, "Line is too long - maximum length is 80 characters"},
    {ERROR_FILE_WRITE, "Could not write to output file - check permissions"},
    {ERROR_FILE_READ, "Could not read from file - check if file exists and permissions"},

    /* Mcro errors */
    {ERROR_MCRO_NO_NAME, "Macro is missing a name"},
    {ERROR_MCRO_EXTRA_TEXT, "Extra text found after macro name - remove extra content"},
    {ERROR_FILE_POSITION, "Could not position file pointer correctly"},
    {ERROR_ENDMACRO_EXTRA_TEXT, "Extra text found after endmcro - remove extra content"},
    {ERROR_MCRO_DUPLICATE, "Macro name already exists - use a different name"},
    {ERROR_MCRO_COPY_FAILED, "Failed to copy macro content"},
    {ERROR_MCRO_EXPANSION, "Failed to expand macro"},
    {ERROR_MCRO_BEFORE_DEF, "Trying to use macro before defining it"},
    {ERROR_MCRO_ILLEGAL_NAME, "Invalid macro name - use only letters and numbers"},

    /* Syntax errors */
    {ERROR_ILLEGAL_OPCODE, "Invalid operation code - check spelling"},
    {ERROR_EXTRA_TEXT, "Extra text found after instruction - remove extra content"},
    {ERROR_ILLEGAL_ARGUMENT, "Invalid argument type or format"},
    {ERROR_MISSING_ARGUMENT, "Missing required argument"},
    {ERROR_MISSING_COMMA, "Missing comma between operands"},
    {ERROR_UNDEFINED_LABEL, "Label not found - check if it's defined"},
    {ERROR_ILLEGAL_ENTRY_LABEL, "Invalid entry label name"},
    {ERROR_ILLEGAL_OPCODE_COMMA, "Incorrect comma placement near operation code"},
    {ERROR_EXCESS_COMMAS, "Too many commas found"},
    {ERROR_MISPLACED_COMMA, "Comma in wrong position"},

    /* Label and register errors */
    {ERROR_ILLEGAL_CHAR_OPCODE, "Invalid character in operation code or label"},
    {ERROR_ILLEGAL_CHAR, "Invalid character found in code"},
    {ERROR_MISSING_NUMBER_COMMA, "Missing comma between numbers"},
    {ERROR_ILLEGAL_LABEL, "Invalid label name - start with letter, use only letters and numbers"},
    {ERROR_MISSING_LABEL_COLON, "Missing colon (:) after label name"},
    {ERROR_ILLEGAL_REGISTER, "Invalid register number - use r0 through r7"},
    {ERROR_COMMA_BEFORE_OPCODE, "Remove comma before operation code"},

    /* Directive errors */
    {ERROR_MISSING_DIRECTIVE_DOT, "Missing dot (.) before directive"},
    {ERROR_DATA_NON_NUMERIC, "Data directive requires numbers only"},
    {ERROR_DATA_TRAILING_COMMA, "Remove extra comma after last number"},
    {ERROR_STRING_MISSING_QUOTE, "String is missing closing quote"},
    {ERROR_STRING_EXTRA_TEXT, "Remove extra text after string"},
    {ERROR_IC_OVERFLOW, "Too many instructions - maximum program size exceeded"},
    {ERROR_LABEL_DUPLICATE, "Label name already used - choose a different name"},
    {ERROR_EXTERN_CONFLICT, "Label cannot be both external and local"},
    {ERROR_DATA_OUT_OF_RANGE, "Number is too big - use smaller value"},
    {ERROR_ILLEGAL_DIRECTIVE, "Invalid directive name"}
};

void print_error(ErrorCode code, int line_number) {
    fprintf(stderr, "Error at line %d: [%s] %s\n", line_number, get_error_name(code), get_error_message(code));
}

void print_error_no_line(ErrorCode code) {
   fprintf(stderr, "Error: [%s] %s\n", get_error_name(code), get_error_message(code));
}

const char* get_error_message(ErrorCode code) {
    int i;
    for (i = 0; errors[i].code != ERROR_ILLEGAL_DIRECTIVE + 1; i++) {
        if (errors[i].code == code) {
            return errors[i].message;
        }
    }
    return "Unknown error";
}

const char* get_error_name(ErrorCode code) {
    switch(code) {
        case ERROR_SUCCESS: return "SUCCESS";
        case ERROR_MEMORY_ALLOCATION: return "MEMORY_ALLOCATION";
        case ERROR_MISSING_AS_FILE: return "MISSING_AS_FILE";
        case ERROR_FILENAME_TOO_LONG: return "FILENAME_TOO_LONG";
        case ERROR_FILE_NOT_EXIST: return "FILE_NOT_EXIST";
        case ERROR_MISSING_AM_FILE: return "MISSING_AM_FILE";
        case ERROR_LINE_TOO_LONG: return "LINE_TOO_LONG";
        case ERROR_FILE_WRITE: return "FILE_WRITE";
        case ERROR_FILE_READ: return "FILE_READ";
        case ERROR_MCRO_NO_NAME: return "MCRO_NO_NAME";
        case ERROR_MCRO_EXTRA_TEXT: return "MCRO_EXTRA_TEXT";
        case ERROR_FILE_POSITION: return "FILE_POSITION";
        case ERROR_ENDMACRO_EXTRA_TEXT: return "ENDMACRO_EXTRA_TEXT";
        case ERROR_MCRO_DUPLICATE: return "MCRO_DUPLICATE";
        case ERROR_MCRO_COPY_FAILED: return "MCRO_COPY_FAILED";
        case ERROR_MCRO_EXPANSION: return "MCRO_EXPANSION";
        case ERROR_MCRO_BEFORE_DEF: return "MCRO_BEFORE_DEF";
        case ERROR_MCRO_ILLEGAL_NAME: return "MCRO_ILLEGAL_NAME";
        case ERROR_ILLEGAL_OPCODE: return "ILLEGAL_OPCODE";
        case ERROR_EXTRA_TEXT: return "EXTRA_TEXT";
        case ERROR_ILLEGAL_ARGUMENT: return "ILLEGAL_ARGUMENT";
        case ERROR_MISSING_ARGUMENT: return "MISSING_ARGUMENT";
        case ERROR_MISSING_COMMA: return "MISSING_COMMA";
        case ERROR_UNDEFINED_LABEL: return "UNDEFINED_LABEL";
        case ERROR_ILLEGAL_ENTRY_LABEL: return "ILLEGAL_ENTRY_LABEL";
        case ERROR_ILLEGAL_OPCODE_COMMA: return "ILLEGAL_OPCODE_COMMA";
        case ERROR_EXCESS_COMMAS: return "EXCESS_COMMAS";
        case ERROR_MISPLACED_COMMA: return "MISPLACED_COMMA";
        case ERROR_ILLEGAL_CHAR_OPCODE: return "ILLEGAL_CHAR_OPCODE";
        case ERROR_ILLEGAL_CHAR: return "ILLEGAL_CHAR";
        case ERROR_MISSING_NUMBER_COMMA: return "MISSING_NUMBER_COMMA";
        case ERROR_ILLEGAL_LABEL: return "ILLEGAL_LABEL";
        case ERROR_MISSING_LABEL_COLON: return "MISSING_LABEL_COLON";
        case ERROR_ILLEGAL_REGISTER: return "ILLEGAL_REGISTER";
        case ERROR_COMMA_BEFORE_OPCODE: return "COMMA_BEFORE_OPCODE";
        case ERROR_MISSING_DIRECTIVE_DOT: return "MISSING_DIRECTIVE_DOT";
        case ERROR_DATA_NON_NUMERIC: return "DATA_NON_NUMERIC";
        case ERROR_DATA_TRAILING_COMMA: return "DATA_TRAILING_COMMA";
        case ERROR_STRING_MISSING_QUOTE: return "STRING_MISSING_QUOTE";
        case ERROR_STRING_EXTRA_TEXT: return "STRING_EXTRA_TEXT";
        case ERROR_IC_OVERFLOW: return "IC_OVERFLOW";
        case ERROR_LABEL_DUPLICATE: return "LABEL_DUPLICATE";
        case ERROR_EXTERN_CONFLICT: return "EXTERN_CONFLICT";
        case ERROR_DATA_OUT_OF_RANGE: return "DATA_OUT_OF_RANGE";
        case ERROR_ILLEGAL_DIRECTIVE: return "ILLEGAL_DIRECTIVE";
        default: return "UNKNOWN_ERROR";
    }
}