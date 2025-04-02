/* Source_files_command_utils.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/command_utils.h"
#include "../Header_Files/label_utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/errors.h"

/* Checks if a token is a valid command name. */
int is_valid_command_name(const char *token)
{
    size_t i;
    if (token == NULL || *token == '\0')
    {
        return FALSE;
    }

    /* compare token with known reserved command words */
    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(token, reserved_words[i]) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/* Validates a command line by checking command name and its parameters. */
ErrorCode is_valid_command(const char *line)
{
    char temp_line[MAX_LINE_LENGTH];
    char *ptr, *temp_ptr;
    char params[2][MAX_LINE_LENGTH];
    char command_name[MAX_LINE_LENGTH];
    int expected_params;
    char saved_char;
    ErrorCode err;

    /* copy the line to a temporary buffer */
    strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
    temp_line[MAX_LINE_LENGTH - 1] = '\0';

    /* advance to the first non-space character */
    ptr = advance_to_next_token(temp_line);

    /* extract the command name */
    temp_ptr = advance_past_token(ptr);

    /* save the command name */
    strncpy(command_name, ptr, temp_ptr - ptr);
    command_name[temp_ptr - ptr] = '\0'; /* null-terminate the command name */

    saved_char = *temp_ptr;
    *temp_ptr = '\0';

    /* determine the expected number of parameters for this command */
    expected_params = get_expected_params(command_name);
    if (expected_params == -1)
    {
        return ERROR_UNKNOWN_COMMAND; /* unknown command */
    }

    *temp_ptr = saved_char;

    /* move pointer past the command name */
    ptr = skip_command_name(ptr);
    ptr = advance_to_next_token(ptr);

    /* validate the parameters based on the expected count */
    err = validate_parameters(ptr, expected_params, params);
    if (err != ERROR_SUCCESS)
    {
        return err;
    }
    else
    {
        err = validate_command_params(command_name, params);
        if (err != ERROR_SUCCESS)
        {
            return err;
        }
    }

    return ERROR_SUCCESS; /* command is valid */
}

/* Returns the expected number of parameters for a command. */
int get_expected_params(const char *command_name)
{
    /* commands expecting 2 parameters */
    if (strcmp(command_name, "mov") == 0 || strcmp(command_name, "cmp") == 0 ||
        strcmp(command_name, "add") == 0 || strcmp(command_name, "sub") == 0 ||
        strcmp(command_name, "lea") == 0)
    {
        return 2;
    }

    /* commands expecting 1 parameter */
    if (strcmp(command_name, "clr") == 0 || strcmp(command_name, "not") == 0 ||
        strcmp(command_name, "inc") == 0 || strcmp(command_name, "dec") == 0 ||
        strcmp(command_name, "jmp") == 0 || strcmp(command_name, "bne") == 0 ||
        strcmp(command_name, "jsr") == 0 || strcmp(command_name, "red") == 0 ||
        strcmp(command_name, "prn") == 0)
    {
        return 1;
    }

    /* commands expecting 0 parameters */
    if (strcmp(command_name, "rts") == 0 || strcmp(command_name, "stop") == 0)
    {
        return 0;
    }

    return -1; /* invalid command name */
}

/* skips over the command name in a line and returns the next token. */
char *skip_command_name(char *line)
{
    while (*line && !isspace((unsigned char)*line))
    {
        line++;
    }
    return advance_to_next_token(line);
}

/* Validates and extracts parameters from a command line. */
ErrorCode validate_parameters(char *line, int expected_params, char params[][MAX_LINE_LENGTH])
{
    char *ptr = line;
    int i, j;
    if (expected_params == 0)
    {
        ptr = advance_to_next_token(ptr);
        return *ptr == '\0' ? ERROR_SUCCESS : ERROR_EXTRA_TEXT_AFTER_COMMAND; /* ensure no extra characters */
    }

    /* extract parameters based on the expected count */
    for (i = 0; i < expected_params; i++)
    {
        ptr = advance_to_next_token(ptr);
        if (*ptr == '\0')
            return ERROR_INVALID_PARAM_COUNT; /* missing parameter */

        /* extract parameter until whitespace or comma */
        j = 0;
        while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',')
        {
            params[i][j++] = *ptr++;
        }
        params[i][j] = '\0';

        /* ensure valid parameter */
        if (j == 0 || strspn(params[i], " \t") == strlen(params[i]))
            return ERROR_INVALID_PARAM_COUNT;

        ptr = advance_to_next_token(ptr);
        if (i == 0 && expected_params == 2 && *ptr != ',')
            return ERROR_MISSING_COMMA; /* missing comma */

        if (i == 0 && expected_params == 2)
            ptr++;
        ptr = advance_to_next_token(ptr);
        if (i == 0 && expected_params == 2 && *ptr == ',')
        {
            return ERROR_CONSECUTIVE_COMMAS; /* consecutive commas */
        }
    }

    return *ptr == '\0' ? ERROR_SUCCESS : ERROR_EXTRA_TEXT_AFTER_COMMAND;
}

/* Validates operand types for a given command. */
ErrorCode validate_command_params(const char *command_name, char params[2][MAX_LINE_LENGTH])
{
    if (strcmp(command_name, "rts") == 0 || strcmp(command_name, "stop") == 0)
    {
        return ERROR_SUCCESS; /* no parameters required, already validated that there is no params */
    }

    if (strcmp(command_name, "clr") == 0 || strcmp(command_name, "not") == 0 ||
        strcmp(command_name, "inc") == 0 || strcmp(command_name, "dec") == 0 ||
        strcmp(command_name, "red") == 0)
    {
        if (validate_register_operand(params[0]) || is_valid_label(params[0]) == ERROR_SUCCESS)
        {
            return ERROR_SUCCESS;
        }
        return ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND;
    }

    if (strcmp(command_name, "mov") == 0 || strcmp(command_name, "add") == 0 || strcmp(command_name, "sub") == 0)
    {
        if (!is_immediate_operand(params[0]) && is_valid_label(params[0]) != ERROR_SUCCESS && !validate_register_operand(params[0]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND;
        }
        if (is_valid_label(params[1]) != ERROR_SUCCESS && !validate_register_operand(params[1]))
        {
            return ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    if (strcmp(command_name, "cmp") == 0)
    {
        if (!is_immediate_operand(params[0]) && is_valid_label(params[0]) != ERROR_SUCCESS && !validate_register_operand(params[0]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND;
        }
        if (!is_immediate_operand(params[1]) && is_valid_label(params[1]) != ERROR_SUCCESS && !validate_register_operand(params[1]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    if (strcmp(command_name, "lea") == 0)
    {
        if (is_valid_label(params[0]) != ERROR_SUCCESS)
        {
            return ERROR_INVALID_DIRECT_FIRST_OPERAND;
        }
        if (is_valid_label(params[1]) != ERROR_SUCCESS && !validate_register_operand(params[1]))
        {
            return ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    if (strcmp(command_name, "jmp") == 0 || strcmp(command_name, "bne") == 0 || strcmp(command_name, "jsr") == 0)
    {
        if (is_valid_label(params[0]) != ERROR_SUCCESS && !(params[0][0] == '&' && is_valid_label(params[0] + 1) == ERROR_SUCCESS))
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
        if (is_valid_label(params[0]) != ERROR_SUCCESS && !validate_register_operand(params[0]))
        {
            return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND;
        }
        return ERROR_SUCCESS;
    }

    return ERROR_UNKNOWN_COMMAND; /* invalid command */
}
