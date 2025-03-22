/* Source_Files/second_pass.c */
#include "../Header_Files/second_pass.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/label_utils.h"
#include "../Header_Files/command_utils.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/globals.h" /* Ensure it contains necessary constants */
#include "../Header_Files/utils.h"   /* Ensure it has advance_to_next_token */
#include "../Header_Files/errors.h"  /* Ensure it has ERROR */

/*Implements the second pass of the assembler. */
int second_pass(FILE *am_file, LabelTable *label_table, VirtualPC *vpc)
{
    char line[MAX_LINE_LENGTH];
    char label[MAX_LABEL_LENGTH];
    char *ptr;
    int line_number = 0;
    size_t label_length = 0;
    ErrorCode err;
    int is_valid_file = TRUE;

    if (am_file == NULL)
    {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }

    rewind(am_file); /* ensure reading from the beginning */

    /* read each line */
    while (fgets(line, MAX_LINE_LENGTH, am_file))
    {
        char *content = line;
        char *colon_pos = NULL;
        int inside_string = 0;
        line_number++;
        /* trim leading spaces */
        ptr = advance_to_next_token(line);

        /* iterate through the line to find a ':' that is not inside quotes */
        while (*ptr)
        {
            if (*ptr == '"')
            {
                inside_string = !inside_string; /* toggle string tracking */
            }
            else if (*ptr == ':' && !inside_string)
            {
                colon_pos = ptr;                 /* found a valid label colon */
                label_length = colon_pos - line; /* set label length */
                break;
            }
            ptr++;
        }

        /* we are in a label declaration line */
        if (colon_pos != NULL)
        {
            strncpy(label, line, label_length);
            label[label_length] = '\0';

            /* extract content after label */
            content = colon_pos + 1;
            content = advance_to_next_token(content);
        }
        else
        {
            content = advance_to_next_token(line); /* No colon found, content is the entire line */
        }

        validate_labels_and_relative_addresses(content, label_table, line_number, &is_valid_file, label);

        /* check for directives that has been treeted in the first pass*/
        if (strncmp(content, ".extern", 7) == 0 ||
            strncmp(content, ".string", 7) == 0 ||
            strncmp(content, ".data", 5) == 0)
        { 
            continue; /* Skip these lines */
        }
        else if (strncmp(content, ".entry", 6) == 0)
        {
            err = is_valid_entry_label(content, label_table); /* also adding "entry" to the label type */
            if (err != ERROR_SUCCESS)
            {
                if (err == ERROR_DUPLICATE_ENTRY_LABEL)
                {
                    print_warning(WARNING_REDUNDANT_ENTRY, line_number);
                }
                else
                {
                    print_error(err, line_number);
                    is_valid_file = FALSE;
                }
            }
            if (colon_pos != NULL) /* if it was part from a label content */
            {
                print_warning(WARNING_LABEL_BEFORE_ENTRY, line_number);
            }
            continue;
        }
    }


    return is_valid_file;
}

/* Processes a command line to validate operands and check for undefined labels. */
void validate_labels_and_relative_addresses(char *content_after_label, LabelTable *label_table, int line_number, int *is_valid_file, char *label)
{
    if (is_valid_command(content_after_label) == ERROR_SUCCESS)
    {
        char *param_ptr = content_after_label;
        param_ptr = advance_to_next_token(param_ptr);
        param_ptr = skip_command_name(param_ptr);
        param_ptr = advance_to_next_token(param_ptr);

        /* iterate over all the operands */
        while (*param_ptr != '\n' && *param_ptr != '\0')
        {
            char operand[MAX_LABEL_LENGTH];
            int i = 0;

            /* extract operand */
            while (*param_ptr != '\0' && !isspace((unsigned char)*param_ptr) && *param_ptr != ',' && *param_ptr != '\n' && i < MAX_LABEL_LENGTH - 1)
            {
                operand[i++] = *param_ptr++;
            }
            operand[i] = '\0'; /* null-terminate the operand */

            /* if the operand is a label (adressing 1) */
            if (operand[0] != '#' && operand[0] != '&' && validate_register_operand(operand) == FALSE)
            {
                Label *label_ptr = get_label_by_name(label_table, operand);
                if (label_ptr == NULL)
                {
                    print_error_with_code(ERROR_UNDEFINED_LABEL, line_number, content_after_label, param_ptr);
                    *is_valid_file = FALSE;
                }
                else 
                {
                    if (strcmp(label_ptr->name, label) == 0)
                    {
                        print_error_with_code(ERROR_LABEL_USED_IN_SAME_LINE, line_number, content_after_label, param_ptr);
                        *is_valid_file = FALSE;
                    }
                }
            }
            else if (operand[0] == '&') /* relative addressing */
            {
                Label *label_ptr = get_label_by_name(label_table, operand + 1);
                if (label_ptr == NULL)
                {
                    print_error_with_code(ERROR_UNDEFINED_LABEL_RELATIVE, line_number, content_after_label, param_ptr);
                    *is_valid_file = FALSE;
                }
                else if (label_ptr->address == 0)
                {
                    print_error_with_code(ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL, line_number, content_after_label, param_ptr);
                    *is_valid_file = FALSE;
                }
                else if (strstr(label_ptr->type, "code") == NULL)
                {
                    print_error_with_code(ERROR_RELATIVE_ADDRESSING_TO_DATA, line_number, content_after_label, param_ptr);
                    *is_valid_file = FALSE;
                }
            }

            /* Skip spaces and commas */
            while (*param_ptr != '\0' && (isspace((unsigned char)*param_ptr) || *param_ptr == ','))
            {
                param_ptr++;
            }
        }
    }
}