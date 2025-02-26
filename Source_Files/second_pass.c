#include "../Header_Files/second_pass.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/label_utils.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/globals.h"  /* Ensure it contains necessary constants */
#include "../Header_Files/utils.h"    /* Ensure it has advance_to_next_token */
#include "../Header_Files/errors.h"   /* Ensure it has ERROR */

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
    char *ptr;
    int line_number = 0;
    ErrorCode err;
    int is_valid_file = TRUE;

    if (am_file == NULL)
    {
        fprintf(stderr, "Error: NULL file pointer provided to second_pass.\n");
        return FALSE;
    }

    rewind(am_file);  /* Ensure reading from the beginning */

    while (fgets(line, MAX_LINE_LENGTH, am_file))
    {
        line_number++;
        /* Trim leading spaces */
        ptr = advance_to_next_token(line);

        /* Skip empty lines */
        if (ptr == NULL || *ptr == '\0')
        {
            continue;
        }

        /* Check for directives */
        if (strncmp(ptr, ".extern", 7) == 0 ||
            strncmp(ptr, ".string", 7) == 0 ||
            strncmp(ptr, ".data", 5) == 0)
        {
            continue; /* Skip these lines */
        }
        else if (strncmp(ptr, ".entry", 6) == 0 && isspace((unsigned char)ptr[6]))
        {   
            err = is_valid_entry_label(ptr, label_table);
            if (err != ERROR_SUCCESS)
            {
                print_error(err, line_number);
                is_valid_file = FALSE;
            }
        }
    }
    return is_valid_file;
}
