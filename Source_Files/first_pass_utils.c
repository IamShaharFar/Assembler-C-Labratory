/*
 * File: first_pass_utils.c
 * Description: Utility functions for handling data storage instructions in the first pass of assembly processing.
 */

#include <string.h> /* Required for strncmp */
#include <stdlib.h> /* Required for strtol */
#include <ctype.h>  /* Required for isspace */
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/label_utils.h"
#include "../Header_Files/first_pass.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/errors.h"

/**
 * @brief Validates if a given line is a proper data storage directive (.data or .string) and checks its syntax correctness.
 *
 * The function checks for the presence and correctness of the directive syntax, including validation of numeric data
 * in the .data directive and string encapsulation in the .string directive. It identifies various formatting errors,
 * such as missing commas, consecutive commas, invalid numeric values, missing quotes, or extra characters.
 *
 * @param line  A pointer to a string containing the line to validate.
 *
 * @return      An ErrorCode indicating the validation result, ERROR_SUCCESS if valid, or a specific error code otherwise.
 */
ErrorCode is_data_storage_instruction(char *line)
{
    if (line == NULL || *line == '\0')
    {
        return ERROR_INVALID_STORAGE_DIRECTIVE;
    }

    /* Now pointing to the first non-space character after directive keyword */
    line = advance_to_next_token(line);

    /* Check for .data directive */
    if (strncmp(line, ".data", 5) == 0 && isspace((unsigned char)line[5]))
    {
        line += 5; /* Advance to the directive content */
        line = advance_to_next_token(line);

        if (*line == '\0')
        {
            return ERROR_INVALID_DATA_NO_NUMBER;
        }

        /* Iterate over the numbers in the .data directive */
        while (*line)
        {
            char *endptr;
            const char *start = line;
            int dot_count = 0;
            const char *ptr = start;
            long num = strtol(line, &endptr, 10); /* Attempt to convert the current token into a numeric value */

            if (endptr == line)
            {
                return ERROR_INVALID_DATA_NON_NUMERIC;
            }

            /* go to the end of the number */
            while (*line && *line != ',')
            {
                line++;
            }

            if (*ptr == '+' || *ptr == '-')
            {
                ptr++;
            }

            /* Check for multiple dots in a number */
            while (*ptr)
            {
                if (*ptr == '.')
                {
                    dot_count++;
                    if (dot_count > 1)
                    {
                        return ERROR_INVALID_DATA_UNEXPECTED_CHAR;
                    }
                }

                /* Check for unexpected characters */
                else if (!isdigit((unsigned char)*ptr))
                {
                    break;
                }
                ptr++;
            }

            /* Check for a dot at the end of a number and for consecutive dots */
            if (dot_count == 1 && ptr != start && *(ptr - 1) != '.')
            {
                return ERROR_INVALID_DATA_REAL_NUMBER;
            }

            if (num > INT_MAX || num < INT_MIN)
            {
                return ERROR_INVALID_DATA_TOO_LARGE; /* Custom error for out-of-bounds integer */
            }

            /* valid number and check for trailing comma */
            line = endptr;
            line = advance_to_next_token(line);

            /* Handle commas between numbers */
            if (*line == ',')
            {
                line++;
                line = advance_to_next_token(line);
                if (*line == ',')
                {
                    return ERROR_CONSECUTIVE_COMMAS;
                }
                else if (*line == '\0')
                {
                    return ERROR_INVALID_DATA_TRAILING_COMMA;
                }
            }
            else if (*line != '\0')
            {
                return ERROR_INVALID_DATA_MISSING_COMMA;
            }
        }
        return ERROR_SUCCESS;
    }
    /* Now checking if the directive is a string (.string) and validating its content */
    else if (strncmp(line, ".string", 7) == 0 && isspace((unsigned char)line[7]))
    {
        line += 7; /* move past the .string */
        line = advance_to_next_token(line);

        if (*line == '\0' || *line == '\n')
        {
            return ERROR_STRING_NO_VALUE;
        }

        else if (*line != '"')
        {
            return ERROR_INVALID_STRING_NO_QUOTE;
        }
        line++;

        /* Iterate over string content */
        while (*line && *line != '"')
        {
            line++;
        }

        if (*line != '"')
        {
            return ERROR_INVALID_STRING_MISSING_END_QUOTE;
        }
        line++;

        line = advance_to_next_token(line);
        if (*line != '\0')
        {
            return ERROR_INVALID_STRING_EXTRA_CHARS;
        }
        return ERROR_SUCCESS;
    }

    /* If the directive is neither .data nor .string, return invalid directive error */
    return ERROR_INVALID_STORAGE_DIRECTIVE;
}

/**
 * @brief the number of elements specified in a data (.data) or string (.string) directive.
 *
 * For .data directives, it counts each numeric value. For .string directives, it counts the characters
 * within the quotes. The function assumes the input pointer initially points to a valid directive.
 *
 * @param ptr  Pointer to the string containing the directive and its content.
 *
 * @return     int The number of data elements or characters in the directive.
 */
int count_data_or_string_elements(char *ptr)
{
    int count = 0;
    ptr = advance_to_next_token(ptr);

    /* Check for .data directive */
    if (strncmp(ptr, ".data", 5) == 0)
    {
        ptr += 5; /* Now ptr points to the first element after the ".data" directive keyword */

        ptr = advance_to_next_token(ptr);

        /* Iterate over the numbers in the .data directive */
        while (*ptr)
        {
            strtol(ptr, &ptr, 10); /* Convert number */
            count++;               /* Successfully parsed one numeric element, incrementing the count */
            ptr = advance_to_next_token(ptr);
            if (*ptr == ',')
            {
                ptr++;
                ptr = advance_to_next_token(ptr);
            }
        }
    }
    /* Check for .string directive */
    else if (strncmp(ptr, ".string", 7) == 0)
    {
        ptr += 7; /* Now ptr points to the start of the string content after the ".string" directive keyword */
        ptr = advance_to_next_token(ptr);

        if (*ptr == '"')
        {
            ptr++;
            while (*ptr && *ptr != '"')
            {
                count++; /* Counting each character within the string quotes */
                ptr++;
            }
            count++; /* null termination is also counting*/
        }
    }
    return count;
}
