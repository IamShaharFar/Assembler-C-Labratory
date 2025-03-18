#include "../Header_Files/second_pass.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/label_utils.h"
#include "../Header_Files/command_utils.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/globals.h" /* Ensure it contains necessary constants */
#include "../Header_Files/utils.h"   /* Ensure it has advance_to_next_token */
#include "../Header_Files/errors.h"  /* Ensure it has ERROR */

/**
 * @file second_pass.c
 * @brief Implements the second pass of the assembler.
 *
 * This function processes an assembly source file (.am) in its second pass,
 * resolving label addresses, handling entry directives, and updating the virtual PC.
 * It performs the following operations:
 *
 * - Skips lines containing `.extern`, `.string`, or `.data` directives.
 * - Validates `.entry` directives and updates label metadata accordingly.
 * - Resolves label addresses, modifying VirtualPC storage as needed.
 * - Detects and reports errors such as undefined labels.
 *
 * If any errors are encountered, the function sets `is_valid_file` to FALSE.
 *
 * @param am_file Pointer to the opened .am file for reading.
 * @param label_table Pointer to the LabelTable structure containing symbol information.
 * @param vpc Pointer to the VirtualPC structure storing assembled machine code.
 *
 * @return int Returns TRUE if the file was processed successfully, FALSE if errors occurred.
 */
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

    rewind(am_file); /* Ensure reading from the beginning */

    while (fgets(line, MAX_LINE_LENGTH, am_file))
    {
        char *content = line;
        char *colon_pos = NULL;
        int inside_string = 0;
        line_number++;
        /* Trim leading spaces */
        ptr = advance_to_next_token(line);
        /* Iterate through the line to find a ':' that is not inside quotes */
        while (*ptr)
        {
            if (*ptr == '"')
            {
                inside_string = !inside_string; /* Toggle string tracking */
            }
            else if (*ptr == ':' && !inside_string)
            {
                colon_pos = ptr;                 /* Found a valid label colon */
                label_length = colon_pos - line; /* Set label length */
                break;
            }
            ptr++;
        }

        if (colon_pos != NULL)
        {
            strncpy(label, line, label_length);
            label[label_length] = '\0';

            /* Extract content after label */
            content = colon_pos + 1;
            while (*content && isspace((unsigned char)*content))
            {
                content++; /* Skip spaces */
            }
            if (strncmp(content, ".entry  ", 7) == 0)
            {
                char *ptr_content = content + 8;
                err = is_valid_entry_label(content, label_table);
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
                ptr_content[strcspn(ptr_content, "\r\n")] = '\0';
                print_warning(WARNING_LABEL_BEFORE_ENTRY, line_number);

                continue;
            }
        }
        else
        {
            content = line; /* No colon found, content is the entire line */
        }

        content = advance_to_next_token(content);
        validate_labels_and_relative_addresses(content, label_table, line_number, &is_valid_file);
        /* Skip empty lines */
        if (content == NULL || *content == '\0')
        {
            continue;
        }
        /* Check for directives */
        if (strncmp(content, ".extern", 7) == 0 ||
            strncmp(content, ".string", 7) == 0 ||
            strncmp(content, ".data", 5) == 0)
        {
            continue; /* Skip these lines */
        }
        else if (strncmp(content, ".entry", 6) == 0 && isspace((unsigned char)content[6]))
        {
            err = is_valid_entry_label(content, label_table);
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
            continue;
        }
    }

    return is_valid_file;
}

/**
 * @brief Processes a command line to validate operands and check for undefined labels.
 *
 * This function takes the content of a command line after the label and performs validation
 * on the operands. It also checks for any undefined labels in the provided label table.
 *
 * @param content_after_label Pointer to the command line content after the label.
 * @param label_table Pointer to the LabelTable structure containing symbol information.
 * @param line_number The current line number in the source file.
 * @param is_valid_file Pointer to the flag indicating if the file is valid.
 * @return void
 */
void validate_labels_and_relative_addresses(char *content_after_label, LabelTable *label_table, int line_number, int *is_valid_file)
{
    if (is_valid_command(content_after_label) == ERROR_SUCCESS)
    {
        char *param_ptr = content_after_label;
        param_ptr = advance_to_next_token(param_ptr);
        param_ptr = skip_command_name(param_ptr);
        param_ptr = advance_to_next_token(param_ptr);

        while (*param_ptr != '\n' && *param_ptr != '\0')
        {
            char operand[MAX_LABEL_LENGTH];
            int i = 0;

            /* Extract operand */
            while (*param_ptr != '\0' && !isspace((unsigned char)*param_ptr) && *param_ptr != ',' && *param_ptr != '\n' && i < MAX_LABEL_LENGTH - 1)
            {
                operand[i++] = *param_ptr++;
            }
            operand[i] = '\0'; /* Null-terminate the operand */

            /* Validate the operand */
            if (operand[0] != '#' && operand[0] != '&' && validate_register_operand(operand) == FALSE)
            {
                if (!label_exists(operand, label_table))
                {
                    print_error_with_code(ERROR_UNDEFINED_LABEL, line_number, content_after_label, param_ptr);
                    *is_valid_file = FALSE;
                }
            }
            else if (operand[0] == '&')
            {
                int label_found = FALSE;
                int i;
                for (i = 0; i < label_table->count; i++)
                {
                    if (strcmp(operand + 1, label_table->labels[i].name) == 0)
                    {
                        label_found = TRUE;
                        if (label_table->labels[i].address == 0)
                        {
                            print_error_with_code(ERROR_RELATIVE_ADDRESSING_EXTERNAL_LABEL, line_number, content_after_label, param_ptr);
                            *is_valid_file = FALSE;
                        }
                        break;
                    }
                }
                if (!label_found)
                {
                    print_error_with_code(ERROR_UNDEFINED_LABEL_RELATIVE, line_number, content_after_label, param_ptr);
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