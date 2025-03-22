/* Source_Files/label_utils.c */

#include <stdio.h>  /* Required for sscanf */
#include <string.h> /* Required for strcmp, strncpy, strncmp */
#include <ctype.h>  /* Required for isalpha, isalnum */
#include "../Header_Files/label_utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/errors.h"

/* Validates whether a given string is a valid label name. */
ErrorCode is_valid_label(const char *label)
{
    size_t i;

    /* move pointer to first non-space character */
    label = advance_to_next_token((char *)label);

    /* check if label starts with a letter */
    if (label == NULL || !isalpha(label[0]))
    {
        return ERROR_ILLEGAL_LABEL_START;
    }

    /* check if label contains only valid characters */
    for (i = 1; label[i] != '\0'; i++)
    {
        if (!isalnum(label[i]) && label[i] != '_')
        {
            return ERROR_ILLEGAL_LABEL_CHAR;
        }
    }

    /* check if label is a register name */
    if (validate_register_operand(label) == TRUE)
    {
        return ERROR_LABEL_IS_REGISTER;
    }

    /* check if label is a reserved word */
    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(label, reserved_words[i]) == 0)
        {
            return ERROR_LABEL_IS_RESERVED_WORD;
        }
    }

    /* check if label length exceeds the allowed limit */
    if (strlen(label) > MAX_LABEL_LENGTH)
    {
        return ERROR_LABEL_TOO_LONG;
    }

    return ERROR_SUCCESS;
}

/* Checks whether a label exists in the label table. */
int label_exists(const char *name, LabelTable *label_table)
{
    int i;
    /* iterate through the label table to find a match */
    for (i = 0; i < label_table->count; i++)
    {
        if (strcmp(label_table->labels[i].name, name) == 0)
        {
            return TRUE; /* label found */
        }
    }
    return FALSE; /* label not found */
}

/* find and return a pointer to a label in the label table by name */
Label *get_label_by_name(LabelTable *label_table, const char *name)
{
    int i;
    if (!label_table || !name)
    {
        return NULL;
    }

    for (i = 0; i < label_table->count; i++)
    {
        if (strcmp(label_table->labels[i].name, name) == 0)
        {
            return &label_table->labels[i];
        }
    }

    return NULL;
}

/* Validates an ".extern" directive and extracts the label. */
ErrorCode is_valid_extern_label(const char *line)
{
    char label[MAX_LINE_LENGTH];
    char temp[MAX_LINE_LENGTH];
    char *ptr;
    ErrorCode err;

    /* copy the input line to a temporary buffer */
    strncpy(temp, line, MAX_LINE_LENGTH - 1);
    temp[MAX_LINE_LENGTH - 1] = '\0'; /* Ensure null termination */

    /* move to the first non-space char */
    ptr = advance_to_next_token(temp);

    /* check if the line mistakenly starts with .entry */
    if (strncmp(ptr, ".entry ", 7) == 0)
    {
        return ERROR_ENTRY_INSTEAD_OF_EXTERN;
    }
    /* check if the line starts with .extern */
    if (strncmp(ptr, ".extern", 7) != 0)
    {
        return ERROR_NOT_EXTERN_LINE;
    }

    /* move past .extern */
    ptr += 7;
    ptr = advance_to_next_token(ptr);

    /* extract label name */
    if (sscanf(ptr, "%s", label) != 1)
    {
        return ERROR_EXTERN_MISSING_LABEL;
    }

    ptr += strlen(label); /* move past the label */
    ptr = advance_to_next_token(ptr);

    /* check if there is extra text after the label */
    if (*ptr != '\0' && *ptr != ';')
    {
        return ERROR_EXTERN_EXTRA_TEXT;
    }

    /* validate the label itself */
    err = is_valid_label(label);
    if (err != ERROR_SUCCESS)
    {
        return err;
    }

    return ERROR_SUCCESS;
}

/* Check if an entry label is valid and update the label table */
ErrorCode is_valid_entry_label(const char *line, LabelTable *label_table)
{
    char label[MAX_LINE_LENGTH];
    char *ptr;
    ErrorCode err;
    Label *label_ptr;

    /* move to the first non-space character */
    ptr = advance_to_next_token((char *)line);

    if (strncmp(ptr, ".entry", 6) != 0)
    {
        return ERROR_NOT_ENTRY_LINE;
    }

    /* move past .entry directive */
    ptr += 6;
    if (*ptr == '\0')
    {
        return ERROR_ENTRY_MISSING_LABEL; /* no valid label found */
    }
    else if(*ptr == ' ' || *ptr == '\t')
    {
        ptr = advance_to_next_token(ptr);
        /* extract label name */
        if (sscanf(ptr, "%s", label) != 1)
        {
            return ERROR_ENTRY_MISSING_LABEL; /* no valid label found */
        }
        ptr += strlen(label);
        ptr = advance_to_next_token(ptr);

        /* check if there is extra text after the label */
        if (*ptr != '\0' && *ptr != ';')
        {
            return ERROR_ENTRY_EXTRA_TEXT;
        }

        /* validate the label itself */
        err = is_valid_label(label);
        if (err != ERROR_SUCCESS)
        {
            return err;
        }

        /* check if label exists in the table and update its type */
        label_ptr = get_label_by_name(label_table, label);
        if (label_ptr != NULL)
        {
            if (label_ptr->address == 0)
            {
                return ERROR_LABEL_NOT_DEFINED_IN_FILE; /* Cannot be an entry with a label that is not defined in the file */
            }

            /* add entry type if not already marked */
            if (strstr(label_ptr->type, "entry") == NULL)
            {
                strncat(label_ptr->type, ", entry", sizeof(label_ptr->type) - strlen(label_ptr->type) - 1);
            }
            else
            {
                return ERROR_DUPLICATE_ENTRY_LABEL;
            }
            return ERROR_SUCCESS;
        }
        return ERROR_UNDEFINED_ENTRY_LABEL; /* label not found in the label table */
    }
    else
    {
        return ERROR_MAYBE_MEANT_ENTRY;
    }
}

/* Adds a new label to the label table. */
ErrorCode add_label(const char *name, int line_number, const char *line, const char *type, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table)
{
    int i;
    ErrorCode err = ERROR_SUCCESS;

    /* check if label table is full */
    if (label_table->count >= MAX_LABELS)
    {
        err = ERROR_MEMORY_ALLOCATION;
    }

    /* check for duplicate labels */
    for (i = 0; i < label_table->count; i++)
    {
        if (strcmp(name, label_table->labels[i].name) == 0)
        {
            if (label_table->labels[i].address == 0)
            {
                return ERROR_LABEL_ALREADY_EXTERN;
            }
            return ERROR_LABEL_DUPLICATE;
        }
    }

    /* check if label name conflicts with a macro name */
    for (i = 0; i < mcro_table->count; i++)
    {
        if (strcmp(name, mcro_table->mcros[i].name) == 0)
        {
            err = ERROR_LABEL_IS_MCRO_NAME;
            return err;
        }
    }

    /* copy label name */
    strncpy(label_table->labels[label_table->count].name, name, MAX_LINE_LENGTH - 1);
    label_table->labels[label_table->count].name[MAX_LINE_LENGTH - 1] = '\0';

    /* set line number */
    label_table->labels[label_table->count].line_number = line_number;

    /* copy the label type and ensure null termination */
    strncpy(label_table->labels[label_table->count].type, type, sizeof(label_table->labels[label_table->count].type) - 1);
    label_table->labels[label_table->count].type[sizeof(label_table->labels[label_table->count].type) - 1] = '\0';

    /* set label address based on type */
    if (strcmp(type, "code") == 0)
    {
        label_table->labels[label_table->count].address = vpc->IC;
    }
    else if (strcmp(type, "data") == 0)
    {
        label_table->labels[label_table->count].address = vpc->DC;
    }
    else
    {
        label_table->labels[label_table->count].address = 0;
    }

    /* increment label count */
    label_table->count++;

    return err;
}
