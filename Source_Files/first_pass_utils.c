/*
 * File: first_pass_utils.c
 * Description: Utility functions for handling data storage instructions in the first pass of assembly processing.
 */

#include <string.h> 
#include <stdlib.h> 
#include <ctype.h>  
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/label_utils.h"
#include "../Header_Files/first_pass.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/errors.h"

/*  Validates if a given line is a proper data storage directive (.data or .string) and checks its syntax correctness.*/
ErrorCode is_data_storage_instruction(char *line)
{
    if (line == NULL || *line == '\0')
    {
        return ERROR_INVALID_STORAGE_DIRECTIVE;
    }

    /* now pointing to the first non-space character after directive keyword */
    line = advance_to_next_token(line);

    /* check for .data directive */
    if (strncmp(line, ".data", 5) == 0 && isspace((unsigned char)line[5]))
    {
        line += 5; /* advance to the directive content */
        line = advance_to_next_token(line);

        if (*line == '\0')
        {
            return ERROR_INVALID_DATA_NO_NUMBER;
        }

        /* iterate over the numbers in the .data directive */
        while (*line)
        {
            char *endptr;
            const char *start = line;
            int dot_count = 0;
            const char *ptr = start;
            long num = strtol(line, &endptr, 10); /* attempt to convert the current token into a numeric value */

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

            /* check for multiple dots in a number to check for real number */
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

                /* check for unexpected characters */
                else if (!isdigit((unsigned char)*ptr))
                {
                    break;
                }
                ptr++;
            }

            /* check for a dot at the end of a number and for consecutive dots means it's a real number */
            if (dot_count == 1 && ptr != start && *(ptr - 1) != '.')
            {
                return ERROR_INVALID_DATA_REAL_NUMBER;
            }

            /* the assembler does'nt support numbers over 24 bits size */
            if (num > INT_MAX || num < INT_MIN)
            {
                return ERROR_INVALID_DATA_TOO_LARGE; 
            }

            /* valid number and check for trailing comma */
            line = endptr;
            line = advance_to_next_token(line);

            /* handle commas between numbers */
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
    /* now checking if the directive is a string (.string) and validating its content */
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

        /* iterate over string content */
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

    /* if the directive is neither .data nor .string, return invalid directive error */
    return ERROR_INVALID_STORAGE_DIRECTIVE;
}

/* The number of elements specified in a data (.data) or string (.string) directive. */
int count_data_or_string_elements(char *line)
{
    int count = 0;
    line = advance_to_next_token(line);

    /* check for .data directive */
    if (strncmp(line, ".data", 5) == 0)
    {
        line += 5; /* now line points to the first element after the ".data" directive keyword */

        line = advance_to_next_token(line);

        /* iterate over the numbers in the .data directive */
        while (*line)
        {
            strtol(line, &line, 10); /* convert number */
            count++;               /* successfully parsed one numeric element, incrementing the count */
            line = advance_to_next_token(line);
            if (*line == ',')
            {
                line++;
                line = advance_to_next_token(line);
            }
        }
    }
    /* check for .string directive */
    else if (strncmp(line, ".string", 7) == 0)
    {
        line += 7; /* now line points to the start of the string content after the ".string" directive keyword */
        line = advance_to_next_token(line);

        if (*line == '"')
        {
            line++;
            while (*line && *line != '"')
            {
                count++; /* counting each character within the string quotes */
                line++;
            }
            count++; /* null termination is also counting*/
        }
    }
    return count;
}
