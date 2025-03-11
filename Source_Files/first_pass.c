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
 * @brief Performs the first pass on an assembly source file to identify and process labels, directives, and commands.
 *
 * This function reads the assembly file line-by-line, identifies labels, verifies their validity, processes data storage
 * directives (.data, .string), command instructions, and external label declarations (.extern). It updates the virtual PC 
 * and label table. Errors encountered during processing are printed.
 *
 * @param fp           Pointer to the assembly (.am) source file to process.
 * @param vpc          Pointer to the VirtualPC struct managing memory addresses.
 * @param label_table  Pointer to the LabelTable struct for storing identified labels.
 * @param mcro_table   Pointer to the McroTable struct containing macro definitions.
 *
 * @return             int TRUE if the file was processed without errors, FALSE otherwise.
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
    int storage_full = FALSE;
    size_t label_length;
    ErrorCode err;

    /* Validate all input pointers */
    if (!fp)
    {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }
    if (!vpc)
    {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }
    if (!label_table)
    {
        print_error_no_line(ERROR_NULL_POINTER);
        return FALSE;
    }
    if (!mcro_table)
    {
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
        ptr = advance_to_next_token(line);         /* Skip leading spaces */

        colon_pos = strchr(line, ':');

        /* Check if the line is a label definition */
        if (colon_pos)
        {
            label_length = colon_pos - ptr;
            strncpy(label, ptr, label_length);
            label[label_length] = '\0'; /* Null-terminate the label */

            content_after_label = colon_pos + 1;

            /* Skip spaces after the colon */
            content_after_label = advance_to_next_token(content_after_label);

            strncpy(content, content_after_label, MAX_LINE_LENGTH - 1);
            content[MAX_LINE_LENGTH - 1] = '\0';

            err = is_valid_label(label);
            if (err == ERROR_SUCCESS)
            {
                err = is_data_storage_instruction(content);
                if (err == ERROR_INVALID_STORAGE_DIRECTIVE) /* not a data storage instruction, maybe a command */
                {
                }
                else /* data storage instruction */
                {
                    if (err != ERROR_SUCCESS) /* invalid data storage instruction */
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                    }
                    else
                    {
                        process_data_or_string_directive(content, vpc, &storage_full); /* process the data storage instruction */
                    }
                    err = add_label(label, line_number, content, "data", vpc, label_table, mcro_table);

                    /* errors like memory allocation or label duplicate or label is macro name */
                    if (err != ERROR_SUCCESS) 
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                    }
                    continue; 
                }

                err = is_valid_command(content);

                /* check if the line start with a valid command name */
                if (err != ERROR_UNKNOWN_COMMAND)
                {
                    if (err != ERROR_SUCCESS) /* invalid command */
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                    }
                    else
                    {
                        process_and_store_command(content, vpc, &storage_full); /* process the command */
                    }
                    err = add_label(label, line_number, content, "code", vpc, label_table, mcro_table);

                    /* errors like memory allocation or label duplicate or label is macro name */
                    if (err != ERROR_SUCCESS)
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                    }
                    continue;
                }

                /* if the line is not a valid command or data storage instruction */
                is_valid_file = FALSE;
                print_error(err, line_number);
                continue;
            }

            /* invalid label */
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

            /* Check if the line is a non valid .extern directive */
            if (err != ERROR_SUCCESS && err != ERROR_NOT_EXTERN_LINE)
            {
                is_valid_file = FALSE;
                print_error(err, line_number);
            }

            /* Check if the line is a valid .extern directive */
            else if (err == ERROR_SUCCESS)
            {
                ptr = advance_to_next_token(line);
                ptr += 7; /* Move past ".extern" */
                ptr = advance_to_next_token(ptr);

                sscanf(ptr, "%s", label); /* Extract the label name */
                err = add_label(label, line_number, "", "external", vpc, label_table, mcro_table);

                /* errors like memory allocation or label duplicate or label is macro name */
                if (err == ERROR_LABEL_DUPLICATE)
                {
                    is_valid_file = FALSE;
                    print_error(ERROR_EXTERN_LABEL_CONFLICT, line_number);
                }
                else if (err != ERROR_SUCCESS)
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
            process_data_or_string_directive(line, vpc, &storage_full); /* process the data storage instruction */
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
            process_and_store_command(line, vpc, &storage_full);
            continue;
        }

        /* invalid line because it's not a valid command or data storage instruction */
        else
        {
            is_valid_file = FALSE;
            print_error(err, line_number);
        }
    }
    if (storage_full)
    {
        is_valid_file = FALSE;
        print_error_no_line(ERROR_VPC_STORAGE_FULL);
    }
    return is_valid_file;
}
