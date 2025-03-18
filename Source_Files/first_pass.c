/* first_pass.c */

/* Standard library headers */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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
    char *colon_pos, *quote_pos;
    char **data_lines = NULL;  /* Dynamic array to store .data/.string lines */
    int data_line_count = 0;   /* Number of stored lines */
    char *content_after_label; /* Pointer to the content after the label (if no label, points to the start of the line) */
    char *ptr;
    int line_number = 0, i;
    int is_valid_file = TRUE;
    int storage_full = FALSE;
    size_t label_length;
    ErrorCode err, error;

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
        quote_pos = strchr(line, '"');

        if (strncmp(line, ".string", 7) != 0)
        {
            if (colon_pos && quote_pos)
            {
                if ((quote_pos - line < colon_pos - line)) /* If the first '"' appears before the first ':' */
                {
                    is_valid_file = FALSE;
                    print_error(ERROR_ILLEGAL_LABEL, line_number);
                    continue;
                }
            }
        }

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
            /* invalid label */
            if (err != ERROR_SUCCESS)
            {
                print_error(err, line_number);
                is_valid_file = FALSE;
            }

            if (strncmp(content, ".extern", 7) == 0)
            {
                if (!isspace((unsigned char)content[7]) && content[7] != '\0')
                {
                    print_error(ERROR_MAYBE_MEANT_EXTERN, line_number);
                    is_valid_file = FALSE;
                    continue;
                }

                err = is_valid_extern_label(content);
                if (err == ERROR_SUCCESS)
                {
                    char *ptr_content = content + 8;
                    ptr_content[strcspn(ptr_content, "\r\n")] = '\0';
                    print_warning(WARNING_LABEL_BEFORE_EXTERN, line_number);
                    err = add_label(ptr_content, line_number, "", "external", vpc, label_table, mcro_table);
                    if (err != ERROR_SUCCESS)
                    {
                        print_error(err, line_number);
                        is_valid_file = FALSE;
                    }
                }
                else
                {
                    print_error(err, line_number);
                    is_valid_file = FALSE;
                }
                continue;
            }
            else if (strncmp(content, ".entry", 6) == 0)
            {
                if (!isspace((unsigned char)content[6]) && content[6] != '\0')
                {
                    print_error(ERROR_MAYBE_MEANT_ENTRY, line_number);
                    is_valid_file = FALSE;
                }
                continue;
            }
            err = is_data_storage_instruction(content);
            if (err == ERROR_INVALID_STORAGE_DIRECTIVE) /* not a data storage instruction, maybe a command */
            {
            }
            else /* data storage instruction */
            {
                error = add_label(label, line_number, content, "data", vpc, label_table, mcro_table);
                if (err != ERROR_SUCCESS)
                {
                    print_error(err, line_number);
                    is_valid_file = FALSE;
                }
                else
                {
                    /* Allocate space for a new line */
                    char **temp = realloc(data_lines, (data_line_count + 1) * sizeof(char *));
                    vpc->DC += count_data_or_string_elements(content);
                    if (!temp)
                    {
                        print_error_no_line(ERROR_MEMORY_ALLOCATION);
                        is_valid_file = FALSE;
                    }
                    else
                    {
                        data_lines = temp;
                        data_lines[data_line_count] = (char *)malloc(strlen(content) + 1);
                        if (!data_lines[data_line_count])
                        {
                            print_error_no_line(ERROR_MEMORY_ALLOCATION);
                            is_valid_file = FALSE;
                        }
                        else
                        {
                            strcpy(data_lines[data_line_count], content);
                            data_line_count++;
                        }
                    }
                }

                /* errors like memory allocation or label duplicate or label is macro name */
                if (error != ERROR_SUCCESS)
                {
                    print_error(error, line_number);
                    is_valid_file = FALSE;
                }
                continue;
            }

            err = is_valid_command(content);

            /* check if the line start with a valid command name */
            if (err != ERROR_UNKNOWN_COMMAND)
            {
                error = add_label(label, line_number, content, "code", vpc, label_table, mcro_table);
                if (err != ERROR_SUCCESS) /* invalid command */
                {
                    print_error(err, line_number);
                    is_valid_file = FALSE;
                }
                else
                {
                    process_and_store_command(content, vpc, &storage_full); /* process the command */
                }

                /* errors like memory allocation or label duplicate or label is macro name */
                if (error != ERROR_SUCCESS)
                {
                    print_error(error, line_number);
                    is_valid_file = FALSE;
                }
                continue;
            }

            /* if the line is not a valid command or data storage instruction */
            print_error(err, line_number);
            is_valid_file = FALSE;
            continue;
        }

        /* Check if the line is an entry or extern declaration */
        if (strncmp(ptr, ".extern", 7) == 0)
        {
            if (!isspace((unsigned char)ptr[7]) && ptr[7] != '\0')
            {
                print_error(ERROR_MAYBE_MEANT_EXTERN, line_number);
                is_valid_file = FALSE;
                continue;
            }

            err = is_valid_extern_label(line);

            /* Check if the line is a non valid .extern directive */
            if (err != ERROR_SUCCESS)
            {
                print_error(err, line_number);
                is_valid_file = FALSE;
            }

            /* Check if the line is a valid .extern directive */
            else
            {
                ptr = advance_to_next_token(line);
                ptr += 7; /* Move past ".extern" */
                ptr = advance_to_next_token(ptr);

                sscanf(ptr, "%s", label); /* Extract the label name */
                err = add_label(label, line_number, "", "external", vpc, label_table, mcro_table);

                /* errors like memory allocation or label duplicate or label is macro name */
                if (err == ERROR_LABEL_DUPLICATE)
                {
                    print_error(ERROR_EXTERN_LABEL_CONFLICT, line_number);
                    is_valid_file = FALSE;
                }
                else if (err != ERROR_SUCCESS)
                {
                    print_error(err, line_number);
                    is_valid_file = FALSE;
                }
            }
            continue;
        }
        if (strncmp(ptr, ".entry", 6) == 0)
        {
            if (!isspace((unsigned char)ptr[6]) && ptr[6] != '\0')
            {
                print_error(ERROR_MAYBE_MEANT_ENTRY, line_number);
                is_valid_file = FALSE;
            }

            continue;
        }

        /* Check if the line is a data or string instruction */
        err = is_data_storage_instruction(line);
        if (err == ERROR_SUCCESS)
        {
            /* process_data_or_string_directive(line, vpc, &storage_full); */ /* process the data storage instruction */
            /* Allocate space for a new line */
            char **temp = realloc(data_lines, (data_line_count + 1) * sizeof(char *));
            vpc->DC += count_data_or_string_elements(line);
            if (!temp)
            {
                print_error_no_line(ERROR_MEMORY_ALLOCATION);
                is_valid_file = FALSE;
            }
            else
            {
                data_lines = temp;
                data_lines[data_line_count] = (char *)malloc(strlen(line) + 1);
                if (!data_lines[data_line_count])
                {
                    print_error_no_line(ERROR_MEMORY_ALLOCATION);
                    is_valid_file = FALSE;
                }
                else
                {
                    strcpy(data_lines[data_line_count], line);
                    data_line_count++;
                }
            }
            continue;
        }
        else if (err != ERROR_INVALID_STORAGE_DIRECTIVE)
        {
            print_error(err, line_number);
            is_valid_file = FALSE;
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
            print_error(err, line_number);
            is_valid_file = FALSE;
        }
    }

    for (i = 0; i < data_line_count; i++)
    {
        process_data_or_string_directive(data_lines[i], vpc, &storage_full);
    }

    for (i = 0; i < label_table->count; i++)
    {
        if (strstr(label_table->labels[i].type, "data") != NULL)
        {
            label_table->labels[i].address = label_table->labels[i].address + vpc->IC;
        }
    }
    /* Free all stored data/string lines */
    for (i = 0; i < data_line_count; i++)
    {
        free(data_lines[i]);
    }
    free(data_lines);

    if (storage_full)
    {
        print_error_no_line(ERROR_VPC_STORAGE_FULL);
        is_valid_file = FALSE;
    }
    return is_valid_file;
}
