/* first_pass_utils.c */
#include "../Header_Files/first_pass_utils.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Header_Files/globals.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/errors.h"
/**
 * @brief Checks if a given token is a valid command or reserved word.
 *
 * This function compares the provided token against the list of reserved words.
 * If the token matches any reserved word, it returns TRUE; otherwise, FALSE.
 *
 * @param token The string to check.
 * @return TRUE if the token is a reserved word, FALSE otherwise.
 */
int is_valid_command_name(const char *token)
{
    size_t i;
    if (token == NULL || *token == '\0')
    {
        return FALSE;
    }

    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(token, reserved_words[i]) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/* Function: is_data_storage_instruction
 * --------------------------------------
 * Checks if a given line is a valid .data or .string directive and validates the format.
 * Returns the appropriate error code.
 *
 * line: The input line to validate.
 *
 * returns: ErrorCode based on validation outcome.
 */
ErrorCode is_data_storage_instruction(const char *line)
{
    if (line == NULL || *line == '\0')
    {
        return ERROR_INVALID_STORAGE_DIRECTIVE;
    }

    /* Skip leading spaces */
    while (isspace((unsigned char)*line))
    {
        line++;
    }

    /* Check for .data directive */
    if (strncmp(line, ".data", 5) == 0 && isspace((unsigned char)line[5]))
    {
        line += 5;
        while (isspace((unsigned char)*line))
            line++;

        if (*line == '\0')
        {
            return ERROR_INVALID_DATA_NO_NUMBER;
        }

        while (*line)
        {
            char *endptr;
            strtol(line, &endptr, 10);
            if (endptr == line)
            {
                return ERROR_INVALID_DATA_NON_NUMERIC;
            }

            line = endptr;
            while (isspace((unsigned char)*line))
                line++;

            if (*line == ',')
            {
                line++;
                while (isspace((unsigned char)*line))
                    line++;
                if (*line == '\0')
                {
                    return ERROR_INVALID_DATA_TRAILING_COMMA;
                }
            }
            else if (*line != '\0')
            {
                return ERROR_INVALID_DATA_UNEXPECTED_CHAR;
            }
        }
        return ERROR_SUCCESS;
    }
    /* Check for .string directive */
    else if (strncmp(line, ".string", 7) == 0 && isspace((unsigned char)line[7]))
    {
        line += 7;
        while (isspace((unsigned char)*line))
            line++;

        if (*line != '"')
        {
            return ERROR_INVALID_STRING_NO_QUOTE;
        }
        line++;

        while (*line && *line != '"')
        {
            line++;
        }

        if (*line != '"')
        {
            return ERROR_INVALID_STRING_MISSING_END_QUOTE;
        }
        line++;

        while (isspace((unsigned char)*line))
            line++;
        if (*line != '\0')
        {
            return ERROR_INVALID_STRING_EXTRA_CHARS;
        }
        return ERROR_SUCCESS;
    }

    return ERROR_INVALID_STORAGE_DIRECTIVE;
}

int is_valid_entry_or_extern_line(char *line)
{
    char *token;
    char label[MAX_LINE_LENGTH];

    /* Skip leading spaces */
    line = advance_to_next_token(line);
    if (!line || *line == '\0')
    {
        return FALSE;
    }

    /* Check if it starts with .entry or .extern and ensure proper spacing */
    if (strncmp(line, ".entry ", 7) == 0)
    {
        token = line + 7;
    }
    else if (strncmp(line, ".extern ", 8) == 0)
    {
        token = line + 8;
    }
    else
    {
        return FALSE;
    }

    /* Skip spaces after directive */
    token = advance_to_next_token(token);
    if (!token || *token == '\0')
    {
        return FALSE; /* No label provided */
    }

    /* Check if the label is valid */
    if (sscanf(token, "%s", label) != 1)
    {
        return FALSE;
    }
    if (!is_valid_label(label))
    {
        printf("this is not a valid label\n");
        return FALSE;
    }

    /* Ensure there is no extra text after label */
    token += strlen(label);
    token = advance_to_next_token(token);
    return (*token == '\0') ? TRUE : FALSE;
}

/**
 * @brief Checks if a given label is valid.
 *
 * A valid label starts with a letter, does not match any reserved word,
 * and is not a register name (r0 to r7).
 *
 * @param label The label to check.
 * @return TRUE if the label is valid, FALSE otherwise.
 */
int is_valid_label(const char *label)
{
    size_t i;
    label = advance_to_next_token((char *)label);

    if (label == NULL || !isalpha(label[0]))
    {
        return FALSE;
    }

    /* Ensure all characters are letters or numbers */
    for (i = 1; label[i] != '\0'; i++)
    {
        if (!isalnum(label[i]) && label[i] != '_')
        {
            return FALSE;
        }
    }

    /* Check if label is a reserved word */
    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(label, reserved_words[i]) == 0)
        {
            return FALSE;
        }
    }

    /* Check if label is a register name (r0 to r7) */
    if (validate_register_operand(label) == TRUE)
    {
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief Checks if a label already exists in the label table.
 *
 * @param name The name of the label to check.
 * @param label_table Pointer to the label table to search in.
 * @return TRUE if the label exists, FALSE otherwise.
 */
int label_exists(const char *name, LabelTable *label_table)
{
    int i;
    for (i = 0; i < label_table->count; i++)
    {
        if (strcmp(label_table->labels[i].name, name) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * @brief Validates an assembly command line for correct syntax and parameters.
 *
 * This function checks if a given command line follows the correct syntax rules:
 * 1. Removes leading spaces.
 * 2. Validates if the command name is a recognized command.
 * 3. Checks the expected number of parameters for the command.
 * 4. Verifies the correct placement of commas and parameters.
 *
 * @param line The command line to validate.
 * @return ERROR_SUCCESS if the command is valid, otherwise returns the appropriate error code.
 */
ErrorCode is_valid_command(const char *line)
{
    char temp_line[MAX_LINE_LENGTH];
    char *ptr, *temp_ptr;
    char params[2][MAX_LINE_LENGTH];
    char command_name[MAX_LINE_LENGTH]; /* To store the command name */
    int expected_params;
    char saved_char;
    ErrorCode err;

    /* Copy the line to a temporary buffer */
    strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
    temp_line[MAX_LINE_LENGTH - 1] = '\0';

    /* Advance to the first non-space character */
    ptr = advance_to_next_token(temp_line);

    temp_ptr = ptr;
    while (*temp_ptr && !isspace((unsigned char)*temp_ptr))
    {
        temp_ptr++;
    }

    /* Save the command name */
    strncpy(command_name, ptr, temp_ptr - ptr);
    command_name[temp_ptr - ptr] = '\0'; /* Null-terminate the command name */

    saved_char = *temp_ptr;
    *temp_ptr = '\0';

    /* Determine how many parameters are expected for this command */
    expected_params = get_expected_params(command_name);
    if (expected_params == -1)
    {
        return ERROR_UNKNOWN_COMMAND; /* Unknown command */
    }

    *temp_ptr = saved_char;

    /* Move pointer past the command name */
    ptr = skip_command_name(ptr);
    ptr = advance_to_next_token(ptr);

    /* Validate the parameters based on the expected count */
    err = validate_parameters(ptr, expected_params, params);
    if (err != ERROR_SUCCESS)
    {
        return err; /* Return specific error for parameter validation */
    }
    else
    {
        err = validate_command_params(command_name, params);
        if (err != ERROR_SUCCESS)
        {
            return err;
        }
    }

    return ERROR_SUCCESS; /* Command is valid */
}

/**
 * @brief Determines the expected number of parameters for a given command.
 *
 * This function checks the command name and returns the number of expected
 * parameters based on predefined command categories.
 *
 * @param command_name The name of the command.
 * @return The number of expected parameters (0, 1, or 2), or -1 if invalid.
 */
int get_expected_params(const char *command_name)
{
    /* Commands expecting 2 parameters */
    if (strcmp(command_name, "mov") == 0 || strcmp(command_name, "cmp") == 0 ||
        strcmp(command_name, "add") == 0 || strcmp(command_name, "sub") == 0 ||
        strcmp(command_name, "lea") == 0)
    {
        return 2;
    }

    /* Commands expecting 1 parameter */
    if (strcmp(command_name, "clr") == 0 || strcmp(command_name, "not") == 0 ||
        strcmp(command_name, "inc") == 0 || strcmp(command_name, "dec") == 0 ||
        strcmp(command_name, "jmp") == 0 || strcmp(command_name, "bne") == 0 ||
        strcmp(command_name, "jsr") == 0 || strcmp(command_name, "red") == 0 ||
        strcmp(command_name, "prn") == 0)
    {
        return 1;
    }

    /* Commands expecting 0 parameters */
    if (strcmp(command_name, "rts") == 0 || strcmp(command_name, "stop") == 0)
    {
        return 0;
    }

    return -1; /* Invalid command name */
}

/**
 * @brief Skips over the command name to the next part of the line.
 *
 * @param line The command line.
 * @return Pointer to the part of the line after the command name.
 */
char *skip_command_name(char *line)
{
    while (*line && !isspace((unsigned char)*line))
    {
        line++;
    }
    return advance_to_next_token(line);
}

/**
 * @brief Validates the parameters of a command based on expected count.
 *
 * @param line The part of the command line containing parameters.
 * @param expected_params The expected number of parameters.
 * @param params Array to store the extracted parameter names.
 * @return ERROR_SUCCESS if parameters are valid, otherwise returns the appropriate error code.
 */
ErrorCode validate_parameters(char *line, int expected_params, char params[][MAX_LINE_LENGTH])
{
    char *ptr = line;
    int i;

    if (expected_params == 0)
    {
        ptr = advance_to_next_token(ptr);
        return *ptr == '\0' ? ERROR_SUCCESS : ERROR_EXTRA_TEXT_AFTER_COMMAND; /* Ensure no extra characters */
    }

    if (expected_params == 1)
    {
        ptr = advance_to_next_token(ptr);
        if (*ptr == '\0')
            return ERROR_INVALID_PARAM_COUNT; /* Missing parameter */

        /* Extract the parameter */
        i = 0;
        while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',')
        {
            params[0][i++] = *ptr++;
        }
        params[0][i] = '\0';

        if (i == 0 || strspn(params[0], " \t") == strlen(params[0]))
            return ERROR_INVALID_PARAM_COUNT;

        ptr = advance_to_next_token(ptr);
        return *ptr == '\0' ? ERROR_SUCCESS : ERROR_EXTRA_TEXT_AFTER_COMMAND; /* Ensure no extra characters */
    }

    if (expected_params == 2)
    {
        /* Extract first parameter */
        ptr = advance_to_next_token(ptr);
        if (*ptr == '\0')
            return ERROR_INVALID_PARAM_COUNT; /* Missing first parameter */

        i = 0;
        while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',')
        {
            params[0][i++] = *ptr++;
        }
        params[0][i] = '\0';
        if (i == 0 || strspn(params[0], " \t") == strlen(params[0]))
            return ERROR_INVALID_PARAM_COUNT;

        ptr = advance_to_next_token(ptr);
        if (*ptr != ',')
            return ERROR_MISSING_COMMA; /* Missing comma */
        ptr++;

        /* Extract second parameter */
        ptr = advance_to_next_token(ptr);
        if (*ptr == ',')
        {
            return ERROR_CONSECUTIVE_COMMAS; /* Consecutive commas */
        }

        i = 0;
        while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',')
        {
            params[1][i++] = *ptr++;
        }
        params[1][i] = '\0';
        if (i == 0 || strspn(params[1], " \t") == strlen(params[1]))
            return ERROR_INVALID_PARAM_COUNT;

        ptr = advance_to_next_token(ptr);
        return *ptr == '\0' ? ERROR_SUCCESS : ERROR_EXTRA_TEXT_AFTER_COMMAND; /* Ensure no extra characters */
    }

    return ERROR_INVALID_PARAM_COUNT; /* Should not reach here */
}

ErrorCode validate_command_params(const char *command_name, char params[2][MAX_LINE_LENGTH])
{
    if (strcmp(command_name, "rts") == 0 || strcmp(command_name, "stop") == 0)
    {
        return ERROR_SUCCESS; /* No parameters required, already validated */
    }

    if (strcmp(command_name, "clr") == 0 || strcmp(command_name, "not") == 0 ||
        strcmp(command_name, "inc") == 0 || strcmp(command_name, "dec") == 0 ||
        strcmp(command_name, "red") == 0)
    {
        if (validate_register_operand(params[0]) || is_valid_label(params[0]))
        {
            return ERROR_SUCCESS;
        }
        return ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND;
    }

    if (strcmp(command_name, "mov") == 0 || strcmp(command_name, "add") == 0 || strcmp(command_name, "sub") == 0)
    {
        if (!(params[0][0] == '#' && strtol(params[0] + 1, NULL, 10) != 0) && !is_valid_label(params[0]) && !validate_register_operand(params[0]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND;
        }
        if (!is_valid_label(params[1]) && !validate_register_operand(params[1]))
        {
            return ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    if (strcmp(command_name, "cmp") == 0)
    {
        if (!(params[0][0] == '#' && strtol(params[0] + 1, NULL, 10) != 0) && !is_valid_label(params[0]) && !validate_register_operand(params[0]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND;
        }
        if (!(params[1][0] == '#' && strtol(params[1] + 1, NULL, 10) != 0) && !is_valid_label(params[1]) && !validate_register_operand(params[1]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    if (strcmp(command_name, "lea") == 0)
    {
        if (!is_valid_label(params[0]))
        {
            return ERROR_INVALID_DIRECT_FIRST_OPERAND;
        }
        if (!is_valid_label(params[1]) && !validate_register_operand(params[1]))
        {
            return ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    if (strcmp(command_name, "jmp") == 0 || strcmp(command_name, "bne") == 0 || strcmp(command_name, "jsr") == 0)
    {
        if (!is_valid_label(params[0]) && !(params[0][0] == '&' && is_valid_label(params[0] + 1)))
        {
            return ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    if (strcmp(command_name, "prn") == 0)
    {
        if (params[0][0] == '#')
        {
            char *endptr;
            strtol(params[0] + 1, &endptr, 10);
            if (*endptr != '\0')
            {
                return ERROR_INVALID_IMMEDIATE_OPERAND;
            }
            return ERROR_SUCCESS;
        }
        if (!is_valid_label(params[0]) && !validate_register_operand(params[0]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    return ERROR_UNKNOWN_COMMAND; /* Invalid command */
}

/**
 * @brief Counts elements in a .data or .string directive.
 *
 * @param ptr Pointer to the content 
 * @return Number of elements (integers for .data, characters for .string).
 */
int count_data_or_string_elements(char *ptr) {
    int count = 0;
    
    if (strncmp(ptr, ".data", 5) == 0) {
        ptr += 5;
        ptr = advance_to_next_token(ptr);
        while (*ptr) {
            strtol(ptr, &ptr, 10);
            count++;
            ptr = advance_to_next_token(ptr);
            if (*ptr == ',') {
                ptr++;
                ptr = advance_to_next_token(ptr);
            }
        }
    } else if (strncmp(ptr, ".string", 7) == 0) {
        ptr += 7;
        ptr = advance_to_next_token(ptr);
        if (*ptr == '"') {
            ptr++;
            while (*ptr && *ptr != '"') {
                count++;
                ptr++;
            }
        }
    }
    return count;
}