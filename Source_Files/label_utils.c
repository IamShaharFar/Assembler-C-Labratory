/*
 * File: label_utils.c
 * Description: Utility functions for handling labels in an assembler.
 */

#include <stdio.h>  /* Required for sscanf */
#include <string.h> /* Required for strcmp, strncpy, strncmp */
#include <ctype.h>  /* Required for isalpha, isalnum */
#include "../Header_Files/label_utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/structs.h"

/*
 * Function: is_valid_label
 * ------------------------
 * Validates whether a given string is a valid label name.
 *
 * label: The label string to check.
 *
 * returns: TRUE if the label is valid, FALSE otherwise.
 */
int is_valid_label(const char *label)
{
    size_t i;

    /* Advance to first valid character */
    label = advance_to_next_token((char *)label);

    /* Label must start with an alphabetic character */
    if (label == NULL || !isalpha(label[0]))
    {
        return FALSE;
    }

    /* Ensure all characters are alphanumeric or underscore */
    for (i = 1; label[i] != '\0'; i++)
    {
        if (!isalnum(label[i]) && label[i] != '_')
        {
            return FALSE;
        }
    }

    /* Check if the label is a reserved word */
    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(label, reserved_words[i]) == 0)
        {
            return FALSE;
        }
    }

    /* Check if label is a register name (e.g., r0-r7) */
    if (validate_register_operand(label) == TRUE)
    {
        return FALSE;
    }

    return TRUE;
}

/*
 * Function: label_exists
 * ----------------------
 * Checks whether a label exists in the label table.
 *
 * name: The label name to check.
 * label_table: The table of defined labels.
 *
 * returns: TRUE if the label exists, FALSE otherwise.
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

/*
 * Function: is_valid_extern_label
 * -------------------------------
 * Validates an ".extern" directive and extracts the label.
 *
 * line: The input line containing the directive.
 *
 * returns: ERROR_SUCCESS if the label is valid, otherwise an appropriate error code.
 */
ErrorCode is_valid_extern_label(const char *line)
{
    char label[MAX_LINE_LENGTH];
    char temp[MAX_LINE_LENGTH];
    char *ptr;

    strncpy(temp, line, MAX_LINE_LENGTH - 1);
    temp[MAX_LINE_LENGTH - 1] = '\0'; /* Ensure null termination */

    /* Ensure the line starts with ".extern" */
    ptr = advance_to_next_token(temp);
    if (strncmp(ptr, ".extern", 7) != 0)
    {
        return ERROR_NOT_EXTERN_LINE;
    }

    /* Move past ".extern" */
    ptr += 7;
    ptr = advance_to_next_token(ptr);

    /* Extract label name */
    if (sscanf(ptr, "%s", label) != 1)
    {
        return ERROR_ILLEGAL_LABEL; /* No valid label found */
    }

    return ERROR_SUCCESS;
}

/**
 * @brief Checks if a given line is a valid .entry directive.
 *
 * This function verifies whether a .entry directive has a label that exists in the label table.
 *
 * @param line The line containing the .entry directive.
 * @param label_table Pointer to the label table to check.
 * @return ERROR_SUCCESS if the label exists, otherwise returns an appropriate error code.
 */
ErrorCode is_valid_entry_label(const char *line, LabelTable *label_table)
{
    char label[MAX_LINE_LENGTH];
    char *ptr;
    int i;

    /* Advance to first non-space character */
    ptr = advance_to_next_token((char *)line);

    /* Move past ".entry" */
    ptr += 6;
    ptr = advance_to_next_token(ptr);

    /* Extract the label */
    if (sscanf(ptr, "%s", label) != 1)
    {
        return ERROR_ILLEGAL_LABEL; /* No valid label found */
    }

    /* Check if the label exists in the label table */
    for (i = 0; i < label_table->count; i++)
    {
        if (strcmp(label_table->labels[i].name, label) == 0)
        {
            /* Append ", entry" to the label type if not already present */
            if (strstr(label_table->labels[i].type, "entry") == NULL)
            {
                strncat(label_table->labels[i].type, ", entry", sizeof(label_table->labels[i].type) - strlen(label_table->labels[i].type) - 1);
            }
            return ERROR_SUCCESS; /* Valid entry directive */
        }
    }

    return ERROR_UNDEFINED_LABEL; /* Label not found */
}


/*
 * Function: is_valid_entry_or_extern_line
 * ---------------------------------------
 * Checks if a line is a valid ".entry" or ".extern" directive.
 *
 * line: The input line to validate.
 *
 * returns: TRUE if the line is valid, FALSE otherwise.
 */
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

    /* Check if it starts with ".entry" or ".extern" */
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

    /* Validate label format */
    if (sscanf(token, "%s", label) != 1)
    {
        return FALSE;
    }
    if (!is_valid_label(label))
    {
        return FALSE;
    }

    /* Ensure there is no extra text after label */
    token += strlen(label);
    token = advance_to_next_token(token);

    return (*token == '\0') ? TRUE : FALSE;
}

/**
 * @brief Adds a new label to the label table.
 *
 * This function adds a label if it doesn't already exist and is not a macro name.
 * If the label exists in the label table or matches a macro name, it triggers an error.
 *
 * @param name The name of the label.
 * @param line_number The line number where the label was found.
 * @param line The full line content after the label.
 * @param type The type of the label.
 * @param address The memory address of the label.
 * @param label_table Pointer to the label table.
 * @param mcro_table Pointer to the macro table.
 *
 * @return ERROR_SUCCESS if the label is added successfully, otherwise returns an appropriate error code.
 */
ErrorCode add_label(const char *name, int line_number, const char *line, const char *type, int address, LabelTable *label_table, const McroTable *mcro_table)
{
    int i;

    /* Check if the label table has space for new labels */
    if (label_table->count >= MAX_LABELS)
    {
        return ERROR_MEMORY_ALLOCATION;
    }

    /* Check if the label already exists in the label table */
    if (label_exists(name, label_table))
    {
        fprintf(stderr, "Error at line %d: Label '%s' is already defined.\n", line_number, name);
        return ERROR_LABEL_DUPLICATE;
    }

    /* Check if the label name conflicts with a macro name */
    for (i = 0; i < mcro_table->count; i++)
    {
        if (strcmp(name, mcro_table->mcros[i].name) == 0)
        {
            fprintf(stderr, "Error at line %d: Label '%s' conflicts with a macro name.\n", line_number, name);
            return ERROR_LABEL_IS_MCRO_NAME;
        }
    }

    /* Add the label to the label table */
    strncpy(label_table->labels[label_table->count].name, name, MAX_LINE_LENGTH - 1);
    label_table->labels[label_table->count].name[MAX_LINE_LENGTH - 1] = '\0';
    label_table->labels[label_table->count].line_number = line_number;

    /* Assign the label type and ensure null termination */
    strncpy(label_table->labels[label_table->count].type, type, sizeof(label_table->labels[label_table->count].type) - 1);
    label_table->labels[label_table->count].type[sizeof(label_table->labels[label_table->count].type) - 1] = '\0';

    /* Assign the given address to the label */
    label_table->labels[label_table->count].address = address;

    /* Store the full line associated with the label */
    strncpy(label_table->labels[label_table->count].line, line, MAX_LINE_LENGTH - 1);
    label_table->labels[label_table->count].line[MAX_LINE_LENGTH - 1] = '\0';

    /* Increment label count */
    label_table->count++;

    return ERROR_SUCCESS;
}

/**
 * @brief Prints the names of all macros in the given McroTable.
 *
 * @param mcro_table Pointer to the McroTable containing the macros.
 */
void print_mcro_names(const McroTable *mcro_table)
{
    int i;

    if (mcro_table == NULL)
    {
        fprintf(stderr, "Error: NULL macro table provided.\n");
        return;
    }

    if (mcro_table->count == 0)
    {
        printf("No macros defined.\n");
        return;
    }

    printf("Defined Macros:\n");
    for (i = 0; i < mcro_table->count; i++)
    {
        printf("%d. %s\n", i + 1, mcro_table->mcros[i].name);
    }
}