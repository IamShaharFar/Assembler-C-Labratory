/* Source_files/first_pass.c */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../Header_Files/first_pass.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/label_utils.h"
#include "../Header_Files/command_utils.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/utils.h"

/* Performs the first pass on an assembly source file to identify and process labels, directives, and commands. */
int first_pass(FILE *fp, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table)
{
    char line[MAX_LINE_LENGTH];
    char original_line[MAX_LINE_LENGTH]; /* Ccpy of the original line */
    char label[MAX_LINE_LENGTH];
    char *colon_pos, *quote_pos;
    char **data_lines = NULL;  /* dynamic array to store .data/.string lines to add to the vpc after commands */
    int data_line_count = 0;   /* number of data lines to store */
    char *content_after_label; /* pointer to the content after the label (if no label, points to the start of the line) */
    char *ptr_line;
    int line_number = 0, i;
    int is_valid_file = TRUE;
    int storage_full = FALSE;
    size_t label_length;
    ErrorCode err, error;

    /* validate all input pointers */
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

    /* process the source file line by line */
    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        line_number++;
        strncpy(original_line, line, MAX_LINE_LENGTH - 1);
        original_line[MAX_LINE_LENGTH - 1] = '\0'; /* eesure null-termination */
        ptr_line = advance_to_next_token(line);    /* skip leading spaces */

        colon_pos = strchr(ptr_line, ':');
        quote_pos = strchr(ptr_line, '"');

        if (strncmp(ptr_line, ".string", 7) != 0)
        {
            if (colon_pos && quote_pos)
            {
                if ((quote_pos - line < colon_pos - ptr_line)) /* if the first '"' appears before the first ':' */
                {
                    is_valid_file = FALSE;
                    print_error(ERROR_ILLEGAL_LABEL, line_number);
                    continue;
                }
            }
        }

        /* check if the line is a label definition */
        if (colon_pos)
        {
            label_length = colon_pos - ptr_line;
            strncpy(label, ptr_line, label_length);
            label[label_length] = '\0'; /* null-terminate the label */

            content_after_label = colon_pos + 1; /* get the label content */

            /* skip spaces after the colon */
            content_after_label = advance_to_next_token(content_after_label);
        }
        else
        {
            content_after_label = advance_to_next_token(line);
        }

        /* check if the line is an extern declaration */
        if (strncmp(content_after_label, ".extern", 7) == 0)
        {
            if (colon_pos)
            {
                print_warning(WARNING_LABEL_BEFORE_EXTERN, line_number);
            }

            if (!isspace((unsigned char)content_after_label[7]) && content_after_label[7] != '\0')
            {
                print_error(ERROR_MAYBE_MEANT_EXTERN, line_number);
                is_valid_file = FALSE;
                continue;
            }

            err = is_valid_extern_label(content_after_label);

            /* check if the line is a non valid .extern directive */
            if (err != ERROR_SUCCESS)
            {
                print_error(err, line_number);
                is_valid_file = FALSE;
            }
            else /* valid extern directive */
            {
                content_after_label = advance_to_next_token(content_after_label);
                content_after_label += 7; /* move past ".extern" */
                content_after_label = advance_to_next_token(content_after_label);

                sscanf(content_after_label, "%s", label); /* extract the label name */
                err = add_label(label, line_number, "", "external", vpc, label_table, mcro_table);

                if (err != ERROR_SUCCESS)
                {
                    print_error(err, line_number);
                    is_valid_file = FALSE;
                }
                else
                {
                    if (is_non_existing_register(label))
                    {
                        print_warning(WARNING_LABEL_RESEMBLES_INVALID_REGISTER, line_number);
                    }
                }
            }
            continue;
        }
        if (strncmp(content_after_label, ".entry", 6) == 0)
        {
            continue;
        }

        /* check if the line is a data or string instruction */
        err = is_data_storage_instruction(content_after_label);
        if (err != ERROR_INVALID_STORAGE_DIRECTIVE) /* a data/string line */
        {
            if (colon_pos)
            {
                error = is_valid_label(label);
                /* invalid label */
                if (error != ERROR_SUCCESS)
                {
                    print_error(error, line_number);
                    is_valid_file = FALSE; /* keep looking for errors at the line*/
                }
                else
                {
                    if (is_non_existing_register(label))
                    {
                        print_warning(WARNING_LABEL_RESEMBLES_INVALID_REGISTER, line_number);
                    }
                    error = add_label(label, line_number, content_after_label, "data", vpc, label_table, mcro_table);
                    if (error != ERROR_SUCCESS)
                    {
                        print_error(error, line_number);
                        is_valid_file = FALSE;
                    }
                }
            }
            if (err != ERROR_SUCCESS) /* non valid data/storage line */
            {
                print_error(err, line_number);
                is_valid_file = FALSE;
            }
            else
            {
                /* allocate memory for a new line */
                char **temp = realloc(data_lines, (data_line_count + 1) * sizeof(char *));
                vpc->DC += count_data_or_string_elements(content_after_label);
                if (!temp)
                {
                    print_error_no_line(ERROR_MEMORY_ALLOCATION);
                    is_valid_file = FALSE;
                }
                else /* add the line that will be proccesed after the commands */
                {
                    data_lines = temp;
                    data_lines[data_line_count] = (char *)malloc(strlen(content_after_label) + 1);
                    if (!data_lines[data_line_count])
                    {
                        print_error_no_line(ERROR_MEMORY_ALLOCATION);
                        is_valid_file = FALSE;
                    }
                    else
                    {
                        strcpy(data_lines[data_line_count], content_after_label);
                        data_line_count++;
                    }
                }
            }
            continue;
        }

        /* check if the line is a valid command */
        err = is_valid_command(content_after_label);
        if (err != ERROR_UNKNOWN_COMMAND)
        {
            if (colon_pos)
            {
                error = is_valid_label(label);
                /* invalid label */
                if (error != ERROR_SUCCESS)
                {
                    print_error(error, line_number);
                    is_valid_file = FALSE; /* keep looking for errors at the line*/
                }
                else
                {
                    if (is_non_existing_register(label))
                    {
                        print_warning(WARNING_LABEL_RESEMBLES_INVALID_REGISTER, line_number);
                    }
                    error = add_label(label, line_number, content_after_label, "code", vpc, label_table, mcro_table);
                    if (error != ERROR_SUCCESS)
                    {
                        print_error(error, line_number);
                        is_valid_file = FALSE;
                    }
                }
            }
            if (err != ERROR_SUCCESS) /* invalid command */
            {
                print_error(err, line_number);
                is_valid_file = FALSE;
            }
            else
            {
                process_and_store_command(content_after_label, vpc, &storage_full); /* process the command */
            }
            continue;
        }

        /* invalid line because it's not a valid command or directive */
        else
        {
            print_error(err, line_number);
            is_valid_file = FALSE;
        }
    }

    /* proccess all the data that didn't proccessed because commands first*/
    for (i = 0; i < data_line_count; i++)
    {
        process_data_or_string_directive(data_lines[i], vpc, &storage_full);
    }

    /* add the final IC to the data labels*/
    for (i = 0; i < label_table->count; i++)
    {
        if (strstr(label_table->labels[i].type, "data") != NULL)
        {
            label_table->labels[i].address = label_table->labels[i].address + vpc->IC;
        }
    }
    /* free all stored data/string lines */
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
