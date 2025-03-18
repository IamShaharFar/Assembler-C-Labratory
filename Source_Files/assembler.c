/*
 * File: assembler.c
 * ------------------
 * This is the main file for the assembler program. It processes
 * assembly files, performs first and second passes, and generates the
 * necessary output files (.ob, .ent, .ext).
 */

#include <stdio.h>
#include <stdlib.h>
#include "../Header_Files/preprocessor.h"       /* process_file() */
#include "../Header_Files/preprocessor_utils.h" /* init_mcro_table() */
#include "../Header_Files/first_pass.h"         /* first_pass() */
#include "../Header_Files/second_pass.h"        /* second_pass() */
#include "../Header_Files/errors.h"             /* print_error_no_line(), print_error() */
#include "../Header_Files/globals.h"            /* MAX_FILENAME_LENGTH, TRUE, FALSE */
#include "../Header_Files/structs.h"            /* VirtualPC, LabelTable, McroTable */
#include "../Header_Files/utils.h"              /* init_virtual_pc(), init_label_table(), init_mcro_table() */
#include "../Header_Files/vpc_utils.h"          /* resolve_and_update_labels() */
#include "../Header_Files/output_builder.h"     /* generate_object_file(), generate_entry_file(), generate_externals_file() */

/* prototype */
void delete_file_if_needed(const char *filename, int success);

int main(int argc, char *argv[])
{
    int i;
    int success;
    FILE *am_file;
    char am_filename[MAX_FILENAME_LENGTH];
    LabelTable label_table;
    McroTable mcro_table;

    /* allocate memory for VirtualPC */
    VirtualPC *vpc = (VirtualPC *)malloc(sizeof(VirtualPC));
    if (!vpc) 
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        exit(EXIT_FAILURE); /* ensures no memory leak */
    }
    

    /* ensure at least one assembly file is provided */
    if (argc < 2)
    {
        print_error(ERROR_MISSING_AS_FILE, 0);
        free(vpc);
        return EXIT_FAILURE;
    }


    /* iterate over each provided assembly file */
    for (i = 1; i < argc; i++)
    {
        success = TRUE;

        /* initialize structures */
        init_virtual_pc(vpc);
        init_label_table(&label_table);
        init_mcro_table(&mcro_table);

        /* generate .am filename for preprocessed file */
        sprintf(am_filename, "%s.am", argv[i]);

        /* preprocess the input file (macro expansion)*/
        if (!process_file(argv[i], &mcro_table))
        {
            print_error_no_line(ERROR_FILE_PROCESSING);
            continue; /* skip this file and move to the next */
        }

        /* open the preprocessed file for further processing */
        am_file = fopen(am_filename, "r");
        if (!am_file)
        {
            print_error_no_line(ERROR_FILE_READ);
            success = FALSE;
        }
        else
        {
            rewind(am_file); /* ensure reading from the start */

            if (!first_pass(am_file, vpc, &label_table, &mcro_table))
            { 
                success = FALSE;
            }
            if (!second_pass(am_file, &label_table, vpc))
            {
                success = FALSE;
            }
            if (success)
            {
                fill_addresses_words(am_file, &label_table, vpc);
            }
            fclose(am_file);
        }

        /* report failure if either pass encountered an error */
        if (!success)
        {
            print_error_no_line(ERROR_ASSEMBLY_FAILED);
        }

        /* if (resolve_and_update_labels(vpc, &label_table) == FALSE)
        {
            success = FALSE;
        } */
        if (success) /* only generate output files if no errors occurred */
        {
            generate_object_file(vpc, argv[i]);
            generate_entry_file(&label_table, argv[i]);
            generate_externals_file(vpc, &label_table, argv[i]);
        }

        /* remove intermediate files if assembly was unsuccessful */
        delete_file_if_needed(am_filename, success);

    }

    /* free allocated memory before program exits */
    free(vpc); 
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

/** 
 * @brief Deletes the .am file if the assembly process failed.
 *
 * @param filename The name of the file to delete.
 * @param success Indicates whether the assembly process was successful.
 */
void delete_file_if_needed(const char *filename, int success)
{
    if (!success && remove(filename) != 0) 
    {
        print_error_no_line(ERROR_FILE_DELETE);
    }
}
