/* errors.c */
#include <stdio.h>
#include "../Header_Files/errors.h"

const Error errors[] = {
    /* General errors */
    {ERROR_SUCCESS, "ERROR_SUCCESS", "Operation completed successfully"},
    {ERROR_MEMORY_ALLOCATION, "ERROR_MEMORY_ALLOCATION", "Not enough memory available to complete operation"},
    {ERROR_NULL_POINTER, "ERROR_NULL_POINTER", "Null pointer encountered"},
    {ERROR_FILE_PROCESSING, "ERROR_FILE_PROCESSING", "Error in preprocessing file, cannot continue"},
    {ERROR_ASSEMBLY_FAILED, "ERROR_ASSEMBLY_FAILED", "Assembly process failed, could not generate output file"},

    /* File errors */
    {ERROR_MISSING_AS_FILE, "ERROR_MISSING_AS_FILE", "Missing source file - please provide a .as file"},
    {ERROR_FILENAME_TOO_LONG, "ERROR_FILENAME_TOO_LONG", "File name is too long - please use a shorter name"},
    {ERROR_FILE_NOT_EXIST, "ERROR_FILE_NOT_EXIST", "Could not find the specified file"},
    {ERROR_LINE_TOO_LONG, "ERROR_LINE_TOO_LONG", "Line is too long - maximum length is 80 characters"},
    {ERROR_FILE_WRITE, "ERROR_FILE_WRITE", "Could not write to output file - check permissions"},
    {ERROR_FILE_READ, "ERROR_FILE_READ", "Could not read from file - check if file exists and permissions"},

    /* Macro errors */
    {ERROR_MCRO_NO_NAME, "ERROR_MCRO_NO_NAME", "Macro is missing a name"},
    {ERROR_MCRO_DUPLICATE, "ERROR_MCRO_DUPLICATE", "Macro name already exists - use a different name"},
    {ERROR_MCRO_BEFORE_DEF, "ERROR_MCRO_BEFORE_DEF", "Trying to use macro before defining it"},
    {ERROR_MCRO_ILLEGAL_NAME, "ERROR_MCRO_ILLEGAL_NAME", "Invalid macro name - use only letters and numbers"},

    /* Label and register errors */
    {ERROR_LABEL_TOO_LONG, "ERROR_LABEL_TOO_LONG", "Label name is too long - maximum length is 30 characters"},
    {ERROR_ILLEGAL_LABEL_START, "ERROR_ILLEGAL_LABEL_START", "Invalid label name - start with letter, use only letters and numbers for the rest"},
    {ERROR_ILLEGAL_LABEL_CHAR, "ERROR_ILLEGAL_LABEL_CHAR", "Invalid label name - use only letters and numbers"},
    {ERRPR_LABEL_IS_RESERVED_WORD, "ERRPR_LABEL_IS_RESERVED_WORD", "Invalid label name - cannot use reserved words"},
    {ERROR_ILLEGAL_LABEL, "ERROR_ILLEGAL_LABEL", "Invalid label name - start with letter, use only letters and numbers"},
    {ERROR_LABEL_DUPLICATE, "ERROR_LABEL_DUPLICATE", "Duplicate label found - use a different name"},
    {ERROR_LABEL_IS_MCRO_NAME, "ERROR_LABEL_IS_MCRO_NAME", "Label name conflicts with macro name - use a different name"},
    {ERROR_UNDEFINED_LABEL, "ERROR_UNDEFINED_LABEL", "Undefined label - label not found in the label table"},
    {ERROR_UNDEFINED_LABEL_RELATIVE, "ERROR_UNDEFINED_LABEL_RELATIVE", "Undefined label - label not found in the label table for relative addressing"},
    {ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL, "ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL", "Cannot use relative addressing with an external label"},
    {ERROR_LABEL_NOT_DEFINED_IN_FILE, "ERROR_LABEL_NOT_DEFINED_IN_FILE", "Label not defined in the file - cannot use as an entry"},
    {ERROR_LABEL_IS_REGISTER, "ERROR_LABEL_IS_REGISTER", "Invalid label name - cannot use register names"},

    /* Command errors */
    {ERROR_UNKNOWN_COMMAND, "ERROR_UNKNOWN_COMMAND", "Unknown command - not recognized by the assembler"},
    {ERROR_INVALID_PARAM_COUNT, "ERROR_INVALID_PARAM_COUNT", "Missing parameters - check the command syntax"},
    {ERROR_MISSING_COMMA, "ERROR_MISSING_COMMA","Missing comma between parameters - add a comma"},
    {ERROR_CONSECUTIVE_COMMAS, "ERROR_CONSECUTIVE_COMMAS","Consecutive commas detected - remove extra commas"},
    {ERROR_EXTRA_TEXT_AFTER_COMMAND, "ERROR_EXTRA_TEXT_AFTER_COMMAND","Extra text after command - remove unnecessary text"},

    {ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND, "ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND", "Invalid operand. Expected Direct Addressing or Register Address Direct."},
    {ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND, "ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND", "Invalid first operand. Must be Immediate Address, Direct Addressing, or Register Address Direct."},
    {ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND, "ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND", "Invalid second operand. Must be Immediate Address, Direct Addressing, or Register Address Direct."},
    {ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND, "ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND", "Invalid second operand. Must be Direct Addressing or Register Address Direct."},
    {ERROR_INVALID_DIRECT_FIRST_OPERAND, "ERROR_INVALID_DIRECT_FIRST_OPERAND", "Invalid first operand. Must be Direct Addressing."},
    {ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND, "ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND", "Invalid second operand. Must be Direct Addressing or Register Address Direct."},
    {ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND, "ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND", "Invalid operand. Expected Relative Addressing or Direct Addressing."},
    {ERROR_INVALID_IMMEDIATE_OPERAND, "ERROR_INVALID_IMMEDIATE_OPERAND", "Invalid numeric value. Expected Immediate Address."},
    {ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND, "ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND", "Invalid operand. Expected Immediate Address, Direct Addressing, or Register Address Direct."},

    {ERROR_NOT_EXTERN_LINE, "ERROR_NOT_EXTERN_LINE", "Line is not a valid .extern directive."},

    /* Data storage errors */
    {ERROR_INVALID_DATA_NO_NUMBER, "ERROR_INVALID_DATA_NO_NUMBER", "Invalid .data directive: Must be followed by at least one number."},
    {ERROR_INVALID_DATA_NON_NUMERIC, "ERROR_INVALID_DATA_NON_NUMERIC", "Invalid .data directive: Contains a non-numeric value."},
    {ERROR_INVALID_DATA_TRAILING_COMMA, "ERROR_INVALID_DATA_TRAILING_COMMA", "Invalid .data directive: Trailing comma detected."},
    {ERROR_INVALID_DATA_UNEXPECTED_CHAR, "ERROR_INVALID_DATA_UNEXPECTED_CHAR", "Invalid .data directive: Unexpected character found."},
    {ERROR_INVALID_STRING_NO_QUOTE, "ERROR_INVALID_STRING_NO_QUOTE", "Invalid .string directive: Must start with a double quote."},
    {ERROR_INVALID_STRING_MISSING_END_QUOTE, "ERROR_INVALID_STRING_MISSING_END_QUOTE", "Invalid .string directive: Missing closing double quote."},
    {ERROR_INVALID_STRING_EXTRA_CHARS, "ERROR_INVALID_STRING_EXTRA_CHARS", "Invalid .string directive: Unexpected characters after closing quote."},
    {ERROR_INVALID_STORAGE_DIRECTIVE, "ERROR_INVALID_STORAGE_DIRECTIVE", "Invalid storage instruction. Expected .data or .string directive."},
    {ERROR_INVALID_LABEL_CONTENT, "ERROR_INVALID_LABEL_CONTENT", "Label content must be a valid .data/.string directive or a valid command."},

};

void print_error(ErrorCode code, int line_number) {
    fprintf(stderr, "Error at line %d: [%s] %s\n", line_number, get_error_name(code), get_error_message(code));
}

void print_error_no_line(ErrorCode code) {
   fprintf(stderr, "Error: [%s] %s\n", get_error_name(code), get_error_message(code));
}

const char* get_error_message(ErrorCode code) {
    int i;
    for (i = 0; i < sizeof(errors) / sizeof(errors[0]); i++) {
        if (errors[i].code == code) {
            return errors[i].message; 
        }
    }
    return "Unknown error";
}


const char* get_error_name(ErrorCode code) {
    int i;
    for (i = 0; i < sizeof(errors) / sizeof(errors[0]); i++) {
        if (errors[i].code == code) {
            return errors[i].name; 
        }
    }
    return "UNKNOWN_ERROR";
}

