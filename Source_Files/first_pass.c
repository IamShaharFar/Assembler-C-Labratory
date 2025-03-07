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
    char original_line[MAX_LINE_LENGTH];
    char label[MAX_LINE_LENGTH];
    char content[MAX_LINE_LENGTH];
    char *colon_pos;
    char *content_after_label;
    char *ptr;
    int line_number = 0;
    int is_valid_file = TRUE;
    size_t label_length;
    ErrorCode err;

    /* Validate all input pointers */
    if (!fp) {
        printf("Error: first_pass received a NULL file pointer.\n");
        return FALSE;
    }
    if (!vpc) {
        printf("Error: first_pass received a NULL VirtualPC pointer.\n");
        return FALSE;
    }
    if (!label_table) {
        printf("Error: first_pass received a NULL LabelTable pointer.\n");
        return FALSE;
    }
    if (!mcro_table) {
        printf("Error: first_pass received a NULL McroTable pointer.\n");
        return FALSE;
    }

    rewind(fp);

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        if (line[0] == '\0') {
            printf("Error: Empty line encountered at line %d.\n", line_number);
            continue;
        }

        line_number++;
        strncpy(original_line, line, MAX_LINE_LENGTH - 1);
        original_line[MAX_LINE_LENGTH - 1] = '\0';
        ptr = advance_to_next_token(line);
        if (!ptr) {
            printf("Error: advance_to_next_token() returned NULL at line %d.\n", line_number);
            continue;
        }
        colon_pos = strchr(line, ':');
        if (colon_pos)
        {
            label_length = colon_pos - line;
            strncpy(label, line, label_length);
            label[label_length] = '\0';

            content_after_label = colon_pos + 1;
            if (!content_after_label) {
                printf("Error: content_after_label is NULL at line %d.\n", line_number);
                continue;
            }

            while (*content_after_label && isspace((unsigned char)*content_after_label))
                content_after_label++;

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
                        printf("Error adding label %s at line %d.\n", label, line_number);
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
                if (!ptr) {
                    printf("Error: advance_to_next_token() returned NULL at line %d.\n", line_number);
                    is_valid_file = FALSE;
                    continue;
                }

                ptr += 7;
                ptr = advance_to_next_token(ptr);
                if (!ptr) {
                    printf("Error: Pointer is NULL before scanning label at line %d.\n", line_number);
                    is_valid_file = FALSE;
                    continue;
                }

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

/**
 * @brief Prints all labels in the label table.
 * @param label_table Pointer to the LabelTable to print.
 */
void print_labels(const LabelTable *label_table)
{
    int i;
    printf("Labels:\n");
    printf("------------------------------------\n");
    printf("| %-20s | %-6s | %-6s |\n", "Name", "Address", "Type");
    printf("------------------------------------\n");

    for (i = 0; i < label_table->count; i++)
    {
        printf("| %-20s | %-6u | %-6s |\n",
               label_table->labels[i].name,
               label_table->labels[i].address,
               label_table->labels[i].type);
    }

    printf("------------------------------------\n");
}
