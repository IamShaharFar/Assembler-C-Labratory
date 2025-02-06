/* Source_Files/assembler.c */
#include <stdio.h>
#include <stdlib.h>
#include "../Header_Files/errors.h"
#include "../Header_Files/file_processor.h"
#include "../Header_Files/globals.h"

int main(int argc, char *argv[]) {
    int i;
    int success = TRUE;
    
    if (argc < 2) {
        print_error(ERROR_MISSING_AS_FILE, 0);
        return EXIT_FAILURE;
    }

    for (i = 1; i < argc; i++) {
        if (!process_file(argv[i])) {
            success = FALSE;
        }
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}