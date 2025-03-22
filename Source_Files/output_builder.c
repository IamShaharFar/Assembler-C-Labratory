#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/output_builder.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/label_utils.h"
#include "../Header_Files/command_utils.h"

/* Writes the assembled machine code into a .ob file. */
void generate_object_file(VirtualPC *vpc, const char *filename)
{
    char ob_filename[MAX_FILENAME_LENGTH + 4]; /* +4 for ".ob\0" */
    FILE *ob_file;
    uint32_t start_addr = 100;
    uint32_t end_addr = vpc->IC + vpc->DC;
    int i;

    /* construct the .ob filename */
    sprintf(ob_filename, "%s.ob", filename);

    /* open the .ob file for writing */
    ob_file = fopen(ob_filename, "w");
    if (!ob_file)
    {
        print_error_no_line(ERROR_OBJECT_FILE_CREATE);
        return;
    }

    /* write IC - 100 and DC in the first line */
    fprintf(ob_file, "%7d %d\n", vpc->IC - 100, vpc->DC);

    /* write the memory content */
    for (i = start_addr; i < end_addr; i++)
    {
        fprintf(ob_file, "%07d %06x\n", i, vpc->storage[i].value & 0xFFFFFF);
        /* ensure 24-bit representation */
    }

    fclose(ob_file);
    printf("Object file '%s' generated successfully.\n", ob_filename);
}

/**
 * @brief Comparison function for sorting labels by address.
 */
int compare_labels_by_address(const void *a, const void *b)
{
    return ((Label *)a)->address - ((Label *)b)->address;
}

/* Writes the entry labels into a .ent file. */
void generate_entry_file(LabelTable *label_table, const char *filename)
{
    char ent_filename[MAX_FILENAME_LENGTH + 4]; /* +4 for ".ent\0" */
    FILE *ent_file;
    int i;
    Label sorted_labels[100];
    int entry_count = 0;

    /* sort labels by address */
    memcpy(sorted_labels, label_table->labels, label_table->count * sizeof(Label));
    qsort(sorted_labels, label_table->count, sizeof(Label), compare_labels_by_address);

    /* check if there are any entry labels */
    for (i = 0; i < label_table->count; i++)
    {
        if (strstr(sorted_labels[i].type, "entry") != NULL)
        {
            entry_count++;
        }
    }

    /* if no entry labels, do not create the file */
    if (entry_count == 0)
    {
        printf("No entry labels found. Entry file not created.\n");
        return;
    }

    /* construct the .ent filename */
    sprintf(ent_filename, "%s.ent", filename);

    /* open the .ent file for writing */
    ent_file = fopen(ent_filename, "w");
    if (!ent_file)
    {
        print_error_no_line(ERROR_ENTRY_FILE_CREATE);
        return;
    }

    /* write the labels marked as "entry" */
    for (i = 0; i < label_table->count; i++)
    {
        if (strstr(sorted_labels[i].type, "entry") != NULL)
        {
            fprintf(ent_file, "%s %07u\n", sorted_labels[i].name, sorted_labels[i].address);
        }
    }

    fclose(ent_file);
    printf("Entry file '%s' generated successfully.\n", ent_filename);
}

/* Writes the external labels into a .ext file. */
void generate_externals_file(VirtualPC *vpc, LabelTable *label_table, const char *filename)
{
    char ext_filename[MAX_FILENAME_LENGTH + 5]; /* +4 for ".ext\0" */
    FILE *ext_file;
    uint32_t start_addr = 100;
    uint32_t end_addr = vpc->IC + vpc->DC;
    int i;
    int extern_count = 0;

    /* construct the .ext filename */
    sprintf(ext_filename, "%s.ext", filename);

    /* open the .ext file for writing */
    ext_file = fopen(ext_filename, "w");
    if (!ext_file)
    {
        print_error_no_line(ERROR_EXTERNAL_FILE_CREATE);
        return;
    }

    /* scan through VirtualPC storage */

    for (i = start_addr; i < end_addr; i++)
    {
        const char *encoded_str = vpc->storage[i].encoded;

        /* check if encoded string matches a label in the label table */
        Label *label_ptr = get_label_by_name(label_table, encoded_str);
        if (label_ptr != NULL && label_ptr->address == 0)
        {
                /* write to file: label name and address in 7-digit format */
                fprintf(ext_file, "%s %07u\n", encoded_str, i);
                extern_count++;
                break;
        }
    }

    /* if no extern labels, do not create the file */
    if (extern_count == 0)
    {
        fclose(ext_file);
        remove(ext_filename);
        printf("No extern labels found. Externals file not created.\n");
        return;
    }

    fclose(ext_file);
    printf("Externals file '%s' generated successfully.\n", ext_filename);
}

/* Fills address words for label operands in the virtual pc. */
void fill_addresses_words(FILE *am_file, LabelTable *label_table, VirtualPC *vpc)
{
    /* variable Declarations */
    char line[MAX_LINE_LENGTH];
    char params[2][MAX_LINE_LENGTH], command_name[10];
    char *ptr, *ptr_scan, *colon_pos;
    int param_count = 0;
    int address = 100; /* initial address */
    int i;
    int inside_string = FALSE;
    int label_address = 0, value = 0;

    rewind(am_file); /* ensure reading from the beginning */

    while (fgets(line, MAX_LINE_LENGTH, am_file))
    {
        memset(params, 0, sizeof(params));
        param_count = 0;
        colon_pos = NULL;
        inside_string = FALSE;
        
        ptr = advance_to_next_token(line); /* skip leading spaces */
        ptr_scan = ptr;

        /* scan through the line to find a colon that is not inside a string */
        while (*ptr_scan)
        {
            if (*ptr_scan == '"')
            {
                inside_string = !inside_string; /* toggle string state */
            }
            else if (*ptr_scan == ':' && !inside_string)
            {
                colon_pos = ptr_scan; /* valid colon found */
                break;
            }
            ptr_scan++;
        }

        /* if a valid colon was found, move past it */
        if (colon_pos)
        {
            ptr = advance_to_next_token(colon_pos + 1);
        }

        /* process .data or .string directives */
        if (strncmp(ptr, ".data", 5) == 0 || strncmp(ptr, ".string", 5) == 0 ||
            strncmp(ptr, ".entry", 5) == 0 || strncmp(ptr, ".extern", 5) == 0)
        {
            continue;
        }
        else
        {
            sscanf(ptr, "%s", command_name);
            if (is_valid_command_name(command_name))
            {
                address += 1; /* count the word of the command name */
                ptr = advance_past_token(ptr);
                ptr = advance_to_next_token(ptr);

                /* check if there is at least 1 operand*/
                if (*ptr != '\n' && *ptr != '\0' && *ptr != '\r')
                {
                    i = 0;

                    /* save the first operand*/
                    while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',')
                    {
                        params[param_count][i++] = *ptr++;
                    }
                    params[param_count][i] = '\0';
                    i = 0;
                    param_count++;

                    /* if there is another operand*/
                    if (*ptr != '\n' && *ptr != '\0' && *ptr != '\r')
                    {
                        ptr++;
                        ptr = advance_to_next_token(ptr);
                        while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',')
                        {
                            params[param_count][i++] = *ptr++;
                        }
                        params[param_count][i] = '\0';
                        param_count++;
                    }
                }

                for (i = 0; i < param_count; i++)
                {
                    int32_t word_value = vpc->storage[address].value;
                    label_address = 0;
                    value = 0;
                    
                    if (params[i][0] == '&')
                    {
                        Label *label_ptr = get_label_by_name(label_table, params[i] + 1);
                        if (label_ptr != NULL)
                        {
                            label_address = label_ptr->address;
                            value = label_address - (address - 1); /* calculate relative address (-1 to reach command address) */

                            word_value &= ~(0x1FFFFF << 3);        /* clear bits 3-23 */
                            word_value |= (value & 0x1FFFFF) << 3; /* set bits 3-23 with the value*/
                            vpc->storage[address].value = word_value;
                        }
                    }

                    else if (label_exists(params[i], label_table))
                    {
                        Label *label_ptr = get_label_by_name(label_table, params[i]);
                        if (label_ptr != NULL)
                        {
                            label_address = label_ptr->address;

                            vpc->storage[address].value = 0;                        /* clear the value */
                            vpc->storage[address].value |= (label_address & 0x1FFFFF) << 3; /* set bits 3-23 with the value*/
                            vpc->storage[address].value &= ~(1 << 2);               /* set bit 2 to 0 */

                            /* set the E/R bits based on the label type */
                            if (strcmp(label_ptr->type, "external") == 0)
                            {
                                vpc->storage[address].value |= (1 << 0);  /* set bit 0 to 1 */
                                vpc->storage[address].value &= ~(1 << 1); /* set bit 1 to 0 */
                            }
                            else
                            {
                                vpc->storage[address].value &= ~(1 << 0); /* set bit 0 to 0 */
                                vpc->storage[address].value |= (1 << 1);  /* set bit 1 to 1 */
                            }
                        }
                    }
                    if (!validate_register_operand(params[i])) /* non register operands give a word */
                    {
                        address++; 
                    }
                }


            }
        }
    }
}