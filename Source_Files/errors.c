/* errors.c */
#include <stdio.h>
#include <string.h>
#include "../Header_Files/errors.h"
#include "../Header_Files/globals.h"

const Error errors[] = {
    /* General errors */
    {ERROR_SUCCESS, "ERROR_SUCCESS", "Operation completed successfully"},
    {ERROR_MEMORY_ALLOCATION, "ERROR_MEMORY_ALLOCATION", "Not enough memory available to complete operation"},
    {ERROR_NULL_POINTER, "ERROR_NULL_POINTER", "Null pointer encountered"},
    
    /* File-related errors */
    {ERROR_MISSING_AS_FILE, "ERROR_MISSING_AS_FILE", "Missing source file - please provide a .as file"},
    {ERROR_FILENAME_TOO_LONG, "ERROR_FILENAME_TOO_LONG", "File name is too long - please use a shorter name"},
    {ERROR_FILE_NOT_EXIST, "ERROR_FILE_NOT_EXIST", "Could not find the specified file"},
    {ERROR_FILE_WRITE, "ERROR_FILE_WRITE", "Could not write to output file - check permissions"},
    {ERROR_FILE_READ, "ERROR_FILE_READ", "Could not read from file - check if file exists and permissions"},
    {ERROR_FILE_DELETE, "ERROR_FILE_DELETE", "Failed to delete the temporary .am file"},
    {ERROR_FILE_PROCESSING, "ERROR_FILE_PROCESSING", "Error in preprocessing file, cannot continue"},
    {ERROR_OBJECT_FILE_CREATE, "ERROR_OBJECT_FILE_CREATE", "Failed to create object file."},
    {ERROR_ENTRY_FILE_CREATE, "ERROR_ENTRY_FILE_CREATE", "Failed to create entry file."},
    {ERROR_EXTERNAL_FILE_CREATE, "ERROR_EXTERNAL_FILE_CREATE", "Failed to create externals file."},

    
    /* Line length errors */
    {ERROR_LINE_TOO_LONG, "ERROR_LINE_TOO_LONG", "Line is too long - maximum length is 80 characters"},
    
    /* Assembly process errors */
    {ERROR_ASSEMBLY_FAILED, "ERROR_ASSEMBLY_FAILED", "Assembly process failed, could not generate output file"},
    {ERROR_VPC_STORAGE_FULL, "ERROR_VPC_STORAGE_FULL", "VirtualPC storage is full - cannot store additional data"},

    /* Macro errors */
    {ERROR_MCRO_NO_NAME, "ERROR_MCRO_NO_NAME", "Macro is missing a name"},
    {ERROR_MCRO_DUPLICATE, "ERROR_MCRO_DUPLICATE", "Macro name already exists - use a different name"},
    {ERROR_MCRO_BEFORE_DEF, "ERROR_MCRO_BEFORE_DEF", "Trying to use macro before defining it"},
    {ERROR_MCRO_TOO_LONG, "ERROR_MCRO_TOO_LONG", "Macro name exceeds the maximum allowed length."},
    {ERROR_MCRO_IS_REGISTER, "ERROR_MCRO_IS_REGISTER", "Macro name cannot be a register name."},
    {ERROR_MCRO_ILLEGAL_CHAR, "ERROR_MCRO_ILLEGAL_CHAR", "Macro name contains an illegal character."},
    {ERROR_MCRO_ILLEGAL_START, "ERROR_MCRO_ILLEGAL_START", "Macro name cannot start with a non-letter character."},
    {ERROR_MCRO_RESERVED_NAME, "ERROR_MCRO_RESERVED_NAME", "Macro name cannot be a reserved word."},
    {ERROR_MCRO_ILLEGAL_NAME, "ERROR_MCRO_ILLEGAL_NAME", "Invalid macro name - use only letters and numbers"},
    {ERROR_MCRO_UNEXPECTED_TEXT, "ERROR_MCRO_UNEXPECTED_TEXT", "Unexpected text after macro name. Only the macro name should follow 'mcro'."},
    {ERROR_MACRO_CALL_EXTRA_TEXT, "ERROR_MACRO_CALL_EXTRA_TEXT", "Macro call must appear alone on the line or be followed only by a comment."},

    /* Label-related errors */
    {ERROR_LABEL_TOO_LONG, "ERROR_LABEL_TOO_LONG", "Label name is too long - maximum length is 30 characters"},
    {ERROR_ILLEGAL_LABEL_START, "ERROR_ILLEGAL_LABEL_START", "Invalid label name - start with letter, use only letters and numbers for the rest"},
    {ERROR_ILLEGAL_LABEL_CHAR, "ERROR_ILLEGAL_LABEL_CHAR", "Invalid label name - use only letters and numbers"},
    {ERROR_LABEL_IS_RESERVED_WORD, "ERROR_LABEL_IS_RESERVED_WORD", "Invalid label name - cannot use reserved words"},
    {ERROR_ILLEGAL_LABEL, "ERROR_ILLEGAL_LABEL", "Invalid label name - start with letter, use only letters and numbers"},
    {ERROR_LABEL_DUPLICATE, "ERROR_LABEL_DUPLICATE", "Duplicate label found - use a different name"},
    {ERROR_LABEL_IS_MCRO_NAME, "ERROR_LABEL_IS_MCRO_NAME", "Label name conflicts with macro name - use a different name"},
    {ERROR_UNDEFINED_LABEL, "ERROR_UNDEFINED_LABEL", "Label not defined in the file - try declaring it before using"},
    {ERROR_UNDEFINED_ENTRY_LABEL, "ERROR_UNDEFINED_ENTRY_LABEL", "Label not defined in the file - cannot use as an entry"},
    {ERROR_UNDEFINED_LABEL_RELATIVE, "ERROR_UNDEFINED_LABEL_RELATIVE", "Undefined label - label not found in the label table for relative addressing"},
    {ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL, "ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL", "Cannot use relative addressing with an external label"},
    {ERROR_RELATIVE_ADDRESSING_TO_DATA, "ERROR_RELATIVE_ADDRESSING_TO_DATA", "Relative addressing is not allowed for labels pointing to data."},
    {ERROR_LABEL_USED_IN_SAME_LINE, "ERROR_LABEL_USED_IN_SAME_LINE", "Label cannot be used as an operand on the same line it is defined"},
    {ERROR_LABEL_NOT_DEFINED_IN_FILE, "ERROR_LABEL_NOT_DEFINED_IN_FILE", "Label is already declared as .extern and cannot be redefined"},
    {ERROR_LABEL_IS_REGISTER, "ERROR_LABEL_IS_REGISTER", "Invalid label name - cannot use register names"},
    {ERROR_EXTERN_LABEL_CONFLICT, "ERROR_EXTERN_LABEL_CONFLICT", "Extern label conflicts with a label declared in this file"},
    {ERROR_LABEL_ALREADY_EXTERN, "ERROR_LABEL_ALREADY_EXTERN", "Label is already declared as extern and cannot be redefined"},
    {ERROR_DUPLICATE_ENTRY_LABEL, "ERROR_DUPLICATE_ENTRY_LABEL", "Label is already declared as .entry"},

    /* Directive errors */
    {ERROR_ENTRY_MISSING_LABEL, "ERROR_ENTRY_MISSING_LABEL", "Missing label after .entry directive."},
    {ERROR_EXTERN_MISSING_LABEL, "ERROR_EXTERN_MISSING_LABEL", "Missing label after .extern directive."},
    {ERROR_ENTRY_INSTEAD_OF_EXTERN, "ERROR_ENTRY_INSTEAD_OF_EXTERN", "Invalid use of .entry. Expected .extern instead."},
    {ERROR_ENTRY_EXTRA_TEXT, "ERROR_ENTRY_EXTRA_TEXT", "Unexpected text after entry label. Only spaces or a comment are allowed."},
    {ERROR_EXTERN_EXTRA_TEXT, "ERROR_EXTERN_EXTRA_TEXT", "Unexpected text after extern label. Only spaces or a comment are allowed."},
    {ERROR_MAYBE_MEANT_ENTRY, "ERROR_MAYBE_MEANT_ENTRY", "Unexpected characters after '.entry'. Did you mean '.entry <label>'?"},
    {ERROR_MAYBE_MEANT_EXTERN, "ERROR_MAYBE_MEANT_EXTERN", "Unexpected characters after '.extern'. Did you mean '.extern <label>'?"},

    /* Command errors */
    {ERROR_UNKNOWN_COMMAND, "ERROR_UNKNOWN_COMMAND", "Unknown command - not recognized by the assembler"},
    {ERROR_INVALID_PARAM_COUNT, "ERROR_INVALID_PARAM_COUNT", "Missing parameters - check the command syntax"},
    {ERROR_MISSING_COMMA, "ERROR_MISSING_COMMA", "Missing comma between parameters - add a comma"},
    {ERROR_CONSECUTIVE_COMMAS, "ERROR_CONSECUTIVE_COMMAS", "Consecutive commas detected - remove extra commas"},
    {ERROR_EXTRA_TEXT_AFTER_COMMAND, "ERROR_EXTRA_TEXT_AFTER_COMMAND", "Extra text after command - remove unnecessary text"},
    {ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND, "ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND", "Invalid operand. Expected Direct Addressing or Register Address Direct."},
    {ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND, "ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND", "Invalid first operand. Must be Immediate Address, Direct Addressing, or Register Address Direct."},
    {ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND, "ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND", "Invalid second operand. Must be Immediate Address, Direct Addressing, or Register Address Direct."},
    {ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND, "ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND", "Invalid second operand. Must be Direct Addressing or Register Address Direct."},
    {ERROR_INVALID_DIRECT_FIRST_OPERAND, "ERROR_INVALID_DIRECT_FIRST_OPERAND", "Invalid first operand. Must be Direct Addressing."},
    {ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND, "ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND", "Invalid second operand. Must be Direct Addressing or Register Address Direct."},
    {ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND, "ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND", "Invalid operand. Expected Relative Addressing or Direct Addressing."},
    {ERROR_INVALID_IMMEDIATE_OPERAND, "ERROR_INVALID_IMMEDIATE_OPERAND", "Invalid numeric value. Expected Immediate Address."},
    {ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND, "ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND", "Invalid operand. Expected Immediate Address, Direct Addressing, or Register Address Direct."},
    {ERROR_INVALID_DATA_REAL_NUMBER, "ERROR_INVALID_DATA_REAL_NUMBER", "Invalid numeric value. Expected an integer."},
    {ERROR_NOT_EXTERN_LINE, "ERROR_NOT_EXTERN_LINE", "Line is not a valid .extern directive."},
    {ERROR_NOT_ENTRY_LINE, "ERROR_NOT_ENTRY_LINE", "Line is not a valid .entry directive."},

    /* Data storage errors */
    {ERROR_INVALID_DATA_NO_NUMBER, "ERROR_INVALID_DATA_NO_NUMBER", "Invalid .data directive: Must be followed by at least one number."},
    {ERROR_INVALID_DATA_NON_NUMERIC, "ERROR_INVALID_DATA_NON_NUMERIC", "Invalid .data directive: Contains a non-numeric value."},
    {ERROR_INVALID_DATA_TRAILING_COMMA, "ERROR_INVALID_DATA_TRAILING_COMMA", "Invalid .data directive: Trailing comma detected."},
    {ERROR_INVALID_DATA_UNEXPECTED_CHAR, "ERROR_INVALID_DATA_UNEXPECTED_CHAR", "Invalid .data directive: Unexpected character found."},
    {ERROR_INVALID_STRING_NO_QUOTE, "ERROR_INVALID_STRING_NO_QUOTE", "Invalid .string directive: Must start with a double quote."},
    {ERROR_INVALID_STRING_MISSING_END_QUOTE, "ERROR_INVALID_STRING_MISSING_END_QUOTE", "Invalid .string directive: Missing closing double quote."},
    {ERROR_INVALID_STRING_EXTRA_CHARS, "ERROR_INVALID_STRING_EXTRA_CHARS", "Invalid .string directive: Unexpected characters after closing quote."},
    {ERROR_INVALID_STORAGE_DIRECTIVE, "ERROR_INVALID_STORAGE_DIRECTIVE", "Invalid storage instruction. Expected .data or .string directive."},
    {ERROR_INVALID_DATA_MISSING_COMMA, "ERROR_INVALID_DATA_MISSING_COMMA", "Invalid .data directive: Missing comma between numbers."},
    {ERROR_INVALID_LABEL_CONTENT, "ERROR_INVALID_LABEL_CONTENT", "Label content must be a valid .data/.string directive or a valid command."},
    {ERROR_INVALID_DATA_TOO_LARGE, "ERROR_INVALID_DATA_TOO_LARGE", "Integer in .data directive exceeds 24-bit limit."},
    {ERROR_STRING_NO_VALUE, "ERROR_STRING_NO_VALUE", "Missing string value after .string directive."},

};

const Warning warnings[] = {
    {WARNING_LABEL_BEFORE_EXTERN, "WARNING_LABEL_BEFORE_EXTERN", "Label before .extern directive is ignored."},
    {WARNING_LABEL_BEFORE_ENTRY, "WARNING_LABEL_BEFORE_ENTRY", "Label before .entry directive is ignored."},
    {WARNING_REDUNDANT_ENTRY, "WARNING_REDUNDANT_ENTRY", "Label declared multiple times as .entry."},
    {WARNING_LABEL_RESEMBLES_INVALID_REGISTER, "WARNING_LABEL_RESEMBLES_INVALID_REGISTER", "Label name resembles an invalid register (e.g., r9) — valid registers are r0 to r7 and cannot be used as labels."}
};

/**
 * @brief print error message with code and highlight the location in the input
 *
 * @param code error code
 * @param line_number line number where the error occurred
 * @param start pointer to the start of the error location
 * @param end pointer to the end of the error location
 */
void print_error_with_code(ErrorCode code, int line_number, const char *start, const char *end)
{
    fprintf(stderr, "%sError at line %d: [%s] %s \n       %.*s <<<-- ERROR HERE%s\n",
            COLOR_RED, line_number, get_error_name(code), get_error_message(code),
            (int)(end - start), start, COLOR_RESET);
}

/**
 * @brief print error message with line number
 *
 * @param code error code
 * @param line_number line number where the error occurred
 */
void print_error(ErrorCode code, int line_number)
{
    fprintf(stderr, COLOR_RED "Error at line %d: [%s] %s" COLOR_RESET "\n",
            line_number, get_error_name(code), get_error_message(code));
}

/**
 * @brief Prints a warning message with the line number.
 *
 * @param code The warning code.
 * @param line_number The line number where the warning occurs.
 */
void print_warning(WarningCode code, int line_number)
{
    fprintf(stderr, "%sWarning at line %d: [%s] %s%s\n",
            COLOR_YELLOW, line_number, get_warning_name(code), get_warning_message(code), COLOR_RESET);
}

/**
 * @brief print error message without line number
 *
 * @param code error code
 */
void print_error_no_line(ErrorCode code)
{
    fprintf(stderr, "%sError: [%s] %s%s\n",
            COLOR_RED, get_error_name(code), get_error_message(code), COLOR_RESET);
}

/**
 * @brief print warning message without line number
 *
 * @param code warning code
 */
void print_warning_no_line(WarningCode code)
{
    fprintf(stderr, "%sWarning: [%s] %s%s\n",
            COLOR_YELLOW, get_warning_name(code), get_warning_message(code), COLOR_RESET);
}

/**
 * @brief get error message from error code
 *
 * @param code error code
 * @return pointer to error message string
 */
const char *get_error_message(ErrorCode code)
{
    int i;
    for (i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
    {
        if (errors[i].code == code)
        {
            return errors[i].message;
        }
    }
    return "Unknown error";
}

/**
 * @brief get error name from error code
 *
 * @param code error code
 * @return pointer to error name string
 */
const char *get_error_name(ErrorCode code)
{
    int i;
    for (i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
    {
        if (errors[i].code == code)
        {
            return errors[i].name;
        }
    }
    return "UNKNOWN_ERROR";
}

/**
 * @brief Gets the warning name as a string.
 *
 * @param code The warning code.
 * @return The name of the warning.
 */
const char *get_warning_name(WarningCode code)
{
    int i;
    for (i = 0; i < sizeof(warnings) / sizeof(warnings[0]); i++)
    {
        if (warnings[i].code == code)
        {
            return warnings[i].name;
        }
    }
    return "UNKNOWN_WARNING";
}

/**
 * @brief Gets the warning message as a string.
 *
 * @param code The warning code.
 * @return The message of the warning.
 */
const char *get_warning_message(WarningCode code)
{
    int i;
    for (i = 0; i < sizeof(warnings) / sizeof(warnings[0]); i++)
    {
        if (warnings[i].code == code)
        {
            return warnings[i].message;
        }
    }
    return "Unknown warning.";
}