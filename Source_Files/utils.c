/* Source_Files/utils.c */
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../Header_Files/utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/errors.h"

/**
 * @brief Advances the pointer to the next non-whitespace character in the string.
 *
 * This function iterates through the given string and advances the pointer past any
 * whitespace characters (spaces, tabs, newlines, etc.). It stops at the first non-whitespace
 * character or the end of the string.
 *
 * @param str Pointer to the input string.
 * @return Pointer to the first non-whitespace character in the string, or the end of the string if no non-whitespace character is found.
 */
char *advance_to_next_token(char *str)
{
    while (str && *str && isspace((unsigned char)*str))
    {
        str++;
    }
    return str;
}

/**
 * @brief Advances the pointer past the current token in the string.
 *
 * This function iterates through the given string and advances the pointer past the current
 * token, which is defined as a sequence of non-whitespace characters. It stops at the first
 * whitespace character or the end of the string.
 *
 * @param str Pointer to the input string.
 * @return Pointer to the first whitespace character after the token, or the end of the string if no whitespace character is found.
 */
char *advance_past_token(char *str)
{
    while (str && *str && !isspace((unsigned char)*str))
    {
        str++;
    }
    return str;
}

/**
 * @brief Advances the pointer past the current token or comma in the string.
 *
 * This function iterates through the given string and advances the pointer past the current
 * token, which is defined as a sequence of non-whitespace characters and commas. It stops at the first
 * whitespace character, comma, or the end of the string.
 *
 * @param str Pointer to the input string.
 * @return Pointer to the first whitespace character, comma, or the end of the string if no such character is found.
 */
char *advance_past_token_or_comma(char *str)
{
    while (*str && !isspace((unsigned char)*str) && *str != ',')
    {
        str++;
    }
    return str;
}

/**
 * @brief Validates if the given string is a valid register operand.
 *
 * This function checks if the provided string represents a valid register operand.
 * A valid register operand must be exactly two characters long, start with the character 'r',
 * and be followed by a digit between '0' and '7'.
 *
 * @param str Pointer to the input string.
 * @return int Returns TRUE if the string is a valid register operand, FALSE otherwise.
 */
int validate_register_operand(const char *str)
{
    if (!str || strlen(str) != 2)
    {
        return FALSE;
    }

    /* Check if the string starts with 'r' and the second character is a digit between '0' and '7' */
    if (str[0] == 'r' && isdigit(str[1]) && str[1] >= '0' && str[1] <= '7')
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Trims trailing newline, carriage return, space, and tab characters from a string.
 *
 * This function removes any trailing newline (`\n`), carriage return (`\r`), space (` `), and tab (`\t`)
 * characters from the end of the given string. It modifies the string in place.
 *
 * @param str Pointer to the input string.
 */
void trim_newline(char *str)
{
    char *end;

    if (str == NULL || *str == '\0')
        return;

    end = str + strlen(str) - 1;

    /* Trim trailing newline, carriage return, space, and tab characters */
    while (end > str && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t'))
    {
        *end = '\0';
        end--;
    }
}

/**
 * @brief Initializes the label table.
 *
 * This function initializes the label table by setting its count to zero.
 * It ensures that the label table is ready to store labels.
 *
 * @param label_table Pointer to the LabelTable structure to be initialized.
 */
void init_label_table(LabelTable *label_table)
{
    if (label_table != NULL)
    {
        int i;
        label_table->count = 0;

        /* Initialize all labels */
        for (i = 0; i < 100; i++)
        {
            memset(label_table->labels[i].name, 0, sizeof(label_table->labels[i].name));
            memset(label_table->labels[i].line, 0, sizeof(label_table->labels[i].line));
            memset(label_table->labels[i].type, 0, sizeof(label_table->labels[i].type));

            label_table->labels[i].line_number = 0;
            label_table->labels[i].address = 100; /* Default starting address */
        }
    }
    else
    {
        print_error_no_line(ERROR_NULL_POINTER);
    }
}

/**
 * @brief Initializes the virtual PC.
 *
 * This function initializes the virtual PC by setting all storage to zero, setting the instruction counter (IC) to 100,
 * and setting the data counter (DC) to zero. It ensures that the virtual PC is ready to store and execute instructions.
 *
 * @param vpc Pointer to the VirtualPC structure to be initialized.
 */
void init_virtual_pc(VirtualPC *vpc)
{
    memset(vpc->storage, 0, sizeof(vpc->storage)); /* Initialize all storage to 0 */
    vpc->IC = 100;                                 /* Initialize IC to 100 */
    vpc->DC = 0;                                   /* Initialize DC to 0 */
}
