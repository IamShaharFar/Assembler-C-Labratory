/* Source_Files/utils.c */
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../Header_Files/utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/errors.h"

/* Processes an operand and updates the binary representation in the provided words. */
char *advance_to_next_token(char *str)
{
    while (str && *str && isspace((unsigned char)*str))
    {
        str++;
    }
    return str;
}

/* Advances the pointer past the current token in the string. */
char *advance_past_token(char *str)
{
    while (str && *str && !isspace((unsigned char)*str))
    {
        str++;
    }
    return str;
}

/* Advances the pointer past the current token or next comma in the string. */
char *advance_past_token_or_comma(char *str)
{
    while (*str && !isspace((unsigned char)*str) && *str != ',')
    {
        str++;
    }
    return str;
}

/* Validates if the given string is a valid register operand. */
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

/* Checks if the given register is non-existing (invalid). */
int is_non_existing_register(const char *reg)
{
    return (strcmp(reg, "r8") == 0 || strcmp(reg, "r9") == 0);
}

/* Checks if a string represents a valid integer number. */
int is_valid_number(const char *s)
{
    if (*s == '-' || *s == '+')
    {
        s++; /* skip optional sign */
    }

    if (!*s)
    {
        return FALSE; /* no digits after sign */
    }

    while (*s)
    {
        if (!isdigit((unsigned char)*s))
        {
            return FALSE; /* non-digit character found */
        }
        s++;
    }

    return TRUE;
}

/* Checks if a parameter is a valid immediate operand. */
int is_immediate_operand(const char *param)
{
    return param[0] == '#' && is_valid_number(param + 1);
}


/* Trims trailing newline, carriage return, space, and tab characters from a string. */
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

/* Initializes the label table. */
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

/* Initializes the virtual PC. */
void init_virtual_pc(VirtualPC *vpc)
{
    memset(vpc->storage, 0, sizeof(vpc->storage)); /* Initialize all storage to 0 */
    vpc ->last_adress = 100;
    vpc->IC = 100;                                 /* Initialize IC to 100 */
    vpc->DC = 0;                                   /* Initialize DC to 0 */
}
