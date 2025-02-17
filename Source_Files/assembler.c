/* Source_Files/assembler.c */
#include <stdio.h>
#include <stdlib.h>
#include "../Header_Files/errors.h"
#include "../Header_Files/preprocessor.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/first_pass.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/preprocessor_utils.h"

int main(int argc, char *argv[]) {
    int i;
    int success = TRUE;
    FILE *am_file;
    char am_filename[MAX_FILENAME_LENGTH];
    VirtualPC *vpc;  /* Change to pointer */
    LabelTable label_table; /* Declare LabelTable */
    McroTable mcro_table; /* Declare McroTable */

    vpc = (VirtualPC *)malloc(sizeof(VirtualPC));  /* Allocate memory dynamically */
    if (!vpc) {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return EXIT_FAILURE;
    }

    init_virtual_pc(vpc);
    init_label_table(&label_table); /* Initialize LabelTable before use */
    init_mcro_table(&mcro_table); /* Initialize McroTable before use */

    if (argc < 2) {
        print_error(ERROR_MISSING_AS_FILE, 0);
        free(vpc);  /* Free allocated memory before exiting */
        return EXIT_FAILURE;
    }

    for (i = 1; i < argc; i++) {
        if (!process_file(argv[i], &mcro_table)) { /* Pass McroTable to process_file */
            success = FALSE;
        } else {
            sprintf(am_filename, "%s.am", argv[i]);
            am_file = fopen(am_filename, "r");
            if (!am_file) {
                print_error(ERROR_FILE_READ, 0);
                success = FALSE;
            } else {
                if (!first_pass(am_file, vpc, &label_table)) { /* Pass LabelTable to first_pass */
                    printf("Error: First pass failed. The file contains errors and cannot be processed further.\n");
                    success = FALSE;
                }
                fclose(am_file);
            }
        }
    }

    free(vpc);  /* Free allocated memory before exiting */
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
