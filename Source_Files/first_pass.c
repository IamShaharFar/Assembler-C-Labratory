/* first_pass.c */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/first_pass.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/structs.h"

static LabelTable label_table; /* Static label table to be used within the first_pass module */

/**
 * @brief Initializes the label table to an empty state.
 */
void init_label_table()
{
    label_table.count = 0;
}

/**
 * @brief Adds a new label to the label table.
 *
 * This function adds a label if it doesn't already exist. If the label exists,
 * it triggers an error.
 *
 * @param name The name of the label.
 * @param line_number The line number where the label was found.
 * @param line The full line content after the label.
 * @return ERROR_SUCCESS if the label is added successfully, otherwise returns an appropriate error code.
 */
ErrorCode add_label(const char *name, int line_number, const char *line)
{
    if (label_table.count >= MAX_LABELS)
    {
        print_error(ERROR_MEMORY_ALLOCATION, line_number);
        return ERROR_MEMORY_ALLOCATION;
    }

    if (label_exists(name, &label_table))
    {
        print_error(ERROR_LABEL_DUPLICATE, line_number);
        return ERROR_LABEL_DUPLICATE;
    }

    strncpy(label_table.labels[label_table.count].name, name, MAX_LINE_LENGTH - 1);
    label_table.labels[label_table.count].name[MAX_LINE_LENGTH - 1] = '\0';
    label_table.labels[label_table.count].line_number = line_number;

    strncpy(label_table.labels[label_table.count].line, line, MAX_LINE_LENGTH - 1);
    label_table.labels[label_table.count].line[MAX_LINE_LENGTH - 1] = '\0';

    label_table.count++;
    return ERROR_SUCCESS;
}

/**
 * @brief Prints all labels stored in the label table.
 */
void print_label_table()
{
    int i;
    printf("\nLabel Table:\n");
    for (i = 0; i < label_table.count; i++)
    {
        printf("Label: %s, Line Number: %d, Line Content: %s\n",
               label_table.labels[i].name,
               label_table.labels[i].line_number,
               label_table.labels[i].line);
    }
}

/**
 * @brief Processes the assembly file for the first pass to identify labels.
 *
 * This function reads through each line of the file and checks for label definitions.
 * Labels are identified as names followed by a colon (e.g., LABEL:).
 * The labels are stored in a LabelTable structure.
 *
 * @param fp Pointer to the assembly source file.
 */
void first_pass(FILE *fp, VirtualPC *vpc)
{
    char line[MAX_LINE_LENGTH];
    char original_line[MAX_LINE_LENGTH];
    char label[MAX_LINE_LENGTH];
    char content[MAX_LINE_LENGTH];
    char *colon_pos;
    char *content_after_label;
    int line_number = 0;
    size_t label_length;
    ErrorCode err;

    init_label_table(); /* Initialize label table */
    rewind(fp);

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        line_number++;
        strncpy(original_line, line, MAX_LINE_LENGTH - 1); /* Preserve the original line */
        original_line[MAX_LINE_LENGTH - 1] = '\0';

        colon_pos = strchr(line, ':');
        if (colon_pos)
        {
            label_length = colon_pos - line;
            strncpy(label, line, label_length);
            label[label_length] = '\0';

            /* Get the content after the label */
            content_after_label = colon_pos + 1;
            while (isspace((unsigned char)*content_after_label))
            {
                content_after_label++;
            }

            strncpy(content, content_after_label, MAX_LINE_LENGTH - 1);
            content[MAX_LINE_LENGTH - 1] = '\0';

            /* Check if the label is a valid identifier */
            if (is_valid_label(label))
            {
                /* Check if content is a valid .data or .string directive */
                err = is_data_storage_instruction(content);
                if (err == ERROR_SUCCESS)
                {
                    continue;
                }

                /* If not a valid storage instruction, check if it's a valid command */
                err = is_valid_command(content);
                if (err != ERROR_SUCCESS)
                {
                    print_error(ERROR_INVALID_LABEL_CONTENT, line_number);
                    continue;
                }
                err = add_label(label, line_number, content);
                if (err != ERROR_SUCCESS)
                {
                    print_error(err, line_number);
                    continue;;
                }

                continue;
            }
            else
            {
                print_error(ERROR_ILLEGAL_LABEL, line_number);
            }
        }
        /* Check if the line is a data storage instruction */
        if (is_valid_entry_or_extern_line(line) == TRUE)
        {
            continue;
        }
        err = is_data_storage_instruction(line);
        if (err == ERROR_INVALID_STORAGE_DIRECTIVE)
        {
        }
        else if (err == ERROR_SUCCESS)
        {
            continue;
        }
        else
        {
            print_error(err, line_number);
            continue;
        }
        err = is_valid_command(line);
        if (err == ERROR_SUCCESS)
        {
            /* printf("Valid command found at line %d: %s\n", line_number, line); */
        }
        else
        {
            print_error(err, line_number);
        }
    }

    /* Print the label table after processing */
    /* print_label_table(); */
}
