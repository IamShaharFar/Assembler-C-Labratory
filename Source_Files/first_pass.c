/* first_pass.c */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/first_pass.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/vpc_utils.h"

/**
 * @brief Adds a new label to the label table.
 *
 * This function adds a label if it doesn't already exist. If the label exists,
 * it triggers an error.
 *
 * @param name The name of the label.
 * @param line_number The line number where the label was found.
 * @param line The full line content after the label.
 * @param label_table Pointer to the label table.
 * @return ERROR_SUCCESS if the label is added successfully, otherwise returns an appropriate error code.
 */
ErrorCode add_label(const char *name, int line_number, const char *line, const char *type, LabelTable *label_table)
{
    if (label_table->count >= MAX_LABELS)
    {
        return ERROR_MEMORY_ALLOCATION;
    }
    if (label_exists(name, label_table))
    {
        return ERROR_LABEL_DUPLICATE;
    }

    strncpy(label_table->labels[label_table->count].name, name, MAX_LINE_LENGTH - 1);
    label_table->labels[label_table->count].name[MAX_LINE_LENGTH - 1] = '\0';
    label_table->labels[label_table->count].line_number = line_number;

    /* Assign the label type and ensure null termination */
    strncpy(label_table->labels[label_table->count].type, type, sizeof(label_table->labels[label_table->count].type) - 1);
    label_table->labels[label_table->count].type[sizeof(label_table->labels[label_table->count].type) - 1] = '\0';

    /* Assign an address dynamically (example: incrementing based on label count) */
    label_table->labels[label_table->count].address = 100 + (label_table->count * 4);

    strncpy(label_table->labels[label_table->count].line, line, MAX_LINE_LENGTH - 1);
    label_table->labels[label_table->count].line[MAX_LINE_LENGTH - 1] = '\0';

    label_table->count++;
    return ERROR_SUCCESS;
}

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
int first_pass(FILE *fp, VirtualPC *vpc, LabelTable *label_table)
{
    char line[MAX_LINE_LENGTH];
    char original_line[MAX_LINE_LENGTH];
    char label[MAX_LINE_LENGTH];
    char content[MAX_LINE_LENGTH];
    char *colon_pos;
    char *content_after_label;
    int line_number = 0;
    int is_valid_file = TRUE;
    int line_dc = 0;
    size_t label_length;
    ErrorCode err;

    label_table->count = 0; /* Initialize label table */
    rewind(fp);

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        line_number++;
        strncpy(original_line, line, MAX_LINE_LENGTH - 1);
        original_line[MAX_LINE_LENGTH - 1] = '\0';

        colon_pos = strchr(line, ':');
        if (colon_pos)
        {
            label_length = colon_pos - line;
            strncpy(label, line, label_length);
            label[label_length] = '\0';

            content_after_label = colon_pos + 1;
            while (isspace((unsigned char)*content_after_label))
            {
                content_after_label++;
            }

            strncpy(content, content_after_label, MAX_LINE_LENGTH - 1);
            content[MAX_LINE_LENGTH - 1] = '\0';

            if (is_valid_label(label))
            {
                err = is_data_storage_instruction(content);
                if (err == ERROR_SUCCESS)
                {
                    err = add_label(label, line_number, content, "data", label_table);
                    if (err != ERROR_SUCCESS)
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                        continue;
                    }
                    print_data_or_string_binary(content);
                    line_dc = count_data_or_string_elements(content);
                    vpc->DC += line_dc;
                    continue;
                }

                err = is_valid_command(content);
                if (err == ERROR_SUCCESS)
                {
                    err = add_label(label, line_number, content, "code", label_table);
                    if (err != ERROR_SUCCESS)
                    {
                        is_valid_file = FALSE;
                        print_error(err, line_number);
                        continue;
                    }
                    generate_binary_command(content);
                    continue;
                }
                is_valid_file = FALSE;
                print_error(ERROR_INVALID_LABEL_CONTENT, line_number);
            }
            else
            {
                is_valid_file = FALSE;
                print_error(ERROR_ILLEGAL_LABEL, line_number);
            }
        }

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
            print_data_or_string_binary(line);
            continue;
        }
        else
        {
            is_valid_file = FALSE;
            print_error(err, line_number);
            continue;
        }

        err = is_valid_command(line);
        if (err == ERROR_SUCCESS)
        {
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

/**
 * @brief Prints all labels stored in the label table.
 */
/* void print_label_table()
 *{
 *    int i;
 *    printf("\nLabel Table:\n");
 *    for (i = 0; i < label_table.count; i++)
 *    {
 *        printf("Label: %s, Line Number: %d, Line Content: %s\n",
 *               label_table.labels[i].name,
 *               label_table.labels[i].line_number,
 *               label_table.labels[i].line);
 *    }
 *}
 */
