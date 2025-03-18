#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Header_Files/output_builder.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/structs.h"


/**
 * @brief Writes the assembled machine code into a .ob file.
 *
 * The function generates a .ob file in the same directory as the .am file.
 * It writes the IC (instruction counter) and DC (data counter) in the first line.
 * Each following line contains the memory address and its corresponding value in hexadecimal format.
 *
 * @param vpc Pointer to the VirtualPC structure containing the machine code.
 * @param filename The name of the .am file (without extension).
 */
void generate_object_file(const VirtualPC *vpc, const char *filename)
{
    char ob_filename[MAX_FILENAME_LENGTH + 4];  /* +4 for ".ob\0" */
    FILE *ob_file;
    uint32_t start_addr = 100;
    uint32_t end_addr = vpc->IC + vpc->DC;
    int i;

    /* Construct the .ob filename */
    sprintf(ob_filename, "%s.ob", filename);

    /* Open the .ob file for writing */
    ob_file = fopen(ob_filename, "w");
    if (!ob_file)
    {
        fprintf(stderr, "Error: Failed to create object file %s\n", ob_filename);
        return;
    }

    /* Write IC - 100 and DC in the first line */
    fprintf(ob_file, "%7d %d\n", vpc->IC - 100, vpc->DC);

    /* Write the memory content */
    for (i = start_addr; i < end_addr; i++)
    {
        fprintf(ob_file, "%07d %06x\n", i, vpc->storage[i].value & 0xFFFFFF);
/* Ensure 24-bit representation */
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

/**
 * @brief Writes the entry labels into a .ent file.
 *
 * This function creates a .ent file in the same directory as the .am file.
 * It writes labels marked with "entry" in increasing order of their addresses.
 * Each line contains the label's address (padded to 7 digits) and its name.
 *
 * @param label_table Pointer to the LabelTable structure containing label data.
 * @param filename The name of the .am file (without extension).
 */
void generate_entry_file(const LabelTable *label_table, const char *filename)
{
    char ent_filename[MAX_FILENAME_LENGTH + 4]; /* +4 for ".ent\0" */
    FILE *ent_file;
    int i;
    Label sorted_labels[100];
    int entry_count = 0;

    /* Sort labels by address */
    memcpy(sorted_labels, label_table->labels, label_table->count * sizeof(Label));
    qsort(sorted_labels, label_table->count, sizeof(Label), compare_labels_by_address);

    /* Check if there are any entry labels */
    for (i = 0; i < label_table->count; i++)
    {
        if (strstr(sorted_labels[i].type, "entry") != NULL)
        {
            entry_count++;
        }
    }

    /* If no entry labels, do not create the file */
    if (entry_count == 0)
    {
        printf("No entry labels found. Entry file not created.\n");
        return;
    }

    /* Construct the .ent filename */
    sprintf(ent_filename, "%s.ent", filename);

    /* Open the .ent file for writing */
    ent_file = fopen(ent_filename, "w");
    if (!ent_file)
    {
        fprintf(stderr, "Error: Failed to create entry file %s\n", ent_filename);
        return;
    }

    /* Write the labels marked as "entry" */
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

/**
 * @brief Writes the external labels into a .ext file.
 *
 * This function creates a .ext file in the same directory as the .am file.
 * It writes labels that have an address of 0 (indicating external references).
 * Each line contains the label's name (encoded string) and the memory address
 * where it is used (formatted as a 7-digit number).
 *
 * @param vpc Pointer to the VirtualPC structure containing machine code.
 * @param label_table Pointer to the LabelTable structure containing labels.
 * @param filename The name of the .am file (without extension).
 */
void generate_externals_file(const VirtualPC *vpc, const LabelTable *label_table, const char *filename)
{
    char ext_filename[MAX_FILENAME_LENGTH + 4]; /* +4 for ".ext\0" */
    FILE *ext_file;
    uint32_t start_addr = 100;
    uint32_t end_addr = vpc->IC + vpc->DC;
    int i, j;
    int extern_count = 0;

    /* Construct the .ext filename */
    sprintf(ext_filename, "%s.ext", filename);

    /* Open the .ext file for writing */
    ext_file = fopen(ext_filename, "w");
    if (!ext_file)
    {
        fprintf(stderr, "Error: Failed to create externals file %s\n", ext_filename);
        return;
    }

    /* Scan through VirtualPC storage */


    for (i = start_addr; i < end_addr; i++)
    {
        const char *encoded_str = vpc->storage[i].encoded;

        /* Check if encoded string matches a label in the label table */
        for (j = 0; j < label_table->count; j++)
        {
            if (strcmp(encoded_str, label_table->labels[j].name) == 0 &&
                label_table->labels[j].address == 0)
            {
                /* Write to file: label name and address in 7-digit format */
                fprintf(ext_file, "%s %07u\n", encoded_str, i);
                extern_count++;
                break;
            }
        }
    }

    /* If no extern labels, do not create the file */
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
