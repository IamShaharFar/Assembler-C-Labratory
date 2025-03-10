/* first_pass.c */

/* Standard library headers */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Project-specific headers */
#include "../Header_Files/first_pass.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/label_utils.h"
#include "../Header_Files/command_utils.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/utils.h"

/**
 * @brief Processes the assembly file for the first pass to identify labels.
 *
 * This function reads through each line of the file and checks for label definitions.
 * Labels are identified as names followed by a colon (e.g., LABEL:).
 * The labels are stored in a LabelTable structure.
 *
 * @param fp Pointer to the assembly source file.
 * @param vpc Virtual PC pointer.
 * @param label_table Pointer to the label table.
 * @return int Returns TRUE if the file is valid, FALSE otherwise.
 */
int first_pass(FILE *fp, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table)
{
    char line[MAX_LINE_LENGTH];
    char original_line[MAX_LINE_LENGTH]; /* Copy of the original line */
    char label[MAX_LINE_LENGTH];
    char content[MAX_LINE_LENGTH];
    char *colon_pos;
    char *content_after_label; /* Pointer to the content after the label (if no label, points to the start of the line) */
    char *ptr;
    int line_number = 0;
    int is_valid_file = TRUE;
    size_t label_length;
    ErrorCode err;

    /* Validate all input pointers */
    if (!fp) {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }
    if (!vpc) {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }
    if (!label_table) {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }
    if (!mcro_table) {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }

    rewind(fp);

    /* Process the source file line by line */
    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        line_number++;
        strncpy(original_line, line, MAX_LINE_LENGTH - 1);
        original_line[MAX_LINE_LENGTH - 1] = '\0'; /* Ensure null-termination */
        ptr = advance_to_next_token(line); /* Skip leading spaces */

        colon_pos = strchr(line, ':');

        /* Check if the line is a label definition */
        if (colon_pos)
        {
            label_length = colon_pos - line; 
            strncpy(label, line, label_length);
            label[label_length] = '\0'; /* Null-terminate the label */

            content_after_label = colon_pos + 1;

            /* Skip spaces after the colon */
            content_after_label = advance_to_next_token(content_after_label);

            if (content_after_label) {
                strncpy(content, content_after_label, MAX_LINE_LENGTH - 1);
                content[MAX_LINE_LENGTH - 1] = '\0';
            } else {
                content[0] = '\0';
            }

            err = is_valid_label(label);
            if (err == ERROR_SUCCESS)
            {
                err = is_data_storage_instruction(content);
                if (err == ERROR_INVALID_STORAGE_DIRECTIVE)
                {
                }
                else
                {
                    if (err != ERROR_SUCCESS)
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                    }
                    err = add_label(label, line_number, content, "data", vpc, label_table, mcro_table);
                    if (err != ERROR_SUCCESS)
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                        continue;
                    }
                    process_data_or_string_directive(content, vpc);
                    continue;
                }

                err = is_valid_command(content);
                if (err != ERROR_UNKNOWN_COMMAND)
                {
                    if (err != ERROR_SUCCESS)
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                    }
                    err = add_label(label, line_number, content, "code", vpc, label_table, mcro_table);
                    if (err != ERROR_SUCCESS)
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                        continue;
                    }
                    process_and_store_command(content, vpc);
                    continue;
                }
                is_valid_file = FALSE;
                print_error(err, line_number);
                continue;
            }
            else
            {
                is_valid_file = FALSE;
                print_error(err, line_number);
                continue;
            }
        }

        /* Check if the line is an entry or extern declaration */
        if (is_valid_entry_or_extern_line(line))
        {
            err = is_valid_extern_label(line);
            if (err != ERROR_SUCCESS && err != ERROR_NOT_EXTERN_LINE)
            {
                is_valid_file = FALSE;
                print_error(err, line_number);
            }
            else if (err == ERROR_SUCCESS)
            {
                ptr = advance_to_next_token(line);
                ptr += 7;
                ptr = advance_to_next_token(ptr);

                sscanf(ptr, "%s", label);
                err = add_label(label, line_number, "", "external", vpc, label_table, mcro_table);
                if (err != ERROR_SUCCESS)
                {
                    is_valid_file = FALSE;
                    print_error(err, line_number);
                }
            }
            continue;
        }

        /* Check if the line is a data or string instruction */
        err = is_data_storage_instruction(line);
        if (err == ERROR_SUCCESS)
        {
            process_data_or_string_directive(line, vpc);
            continue;
        }
        else if (err != ERROR_INVALID_STORAGE_DIRECTIVE)
        {
            is_valid_file = FALSE;
            print_error(err, line_number);
            continue;
        }

        /* Check if the line is a valid command */
        err = is_valid_command(line);
        if (err == ERROR_SUCCESS)
        {
            process_and_store_command(line, vpc);
            continue;
        }
        else
        {
            is_valid_file = FALSE;
            print_error(err, line_number);
        }
    }
    return is_valid_file;
}
