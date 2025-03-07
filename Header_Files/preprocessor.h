/* preprocessor.h */
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "errors.h"
#include "globals.h"
#include "structs.h"

/**
 * @brief Processes the macros in the given assembly file.
 *
 * This function reads the assembly file line by line, processes macro definitions and their content,
 * and stores them in the provided macro table. It handles memory allocation for reading lines and
 * ensures that lines do not exceed the maximum allowed length. The function also creates an output
 * file with the processed content.
 *
 * @param fp File pointer to the assembly source file.
 * @param file_path Path to the source file.
 * @param mcro_table Pointer to the macro table.
 * @return TRUE (1) if the file is valid and processed successfully, FALSE (0) otherwise.
 */
int process_as_file(FILE *fp, const char *file_path, McroTable *mcro_table);

/**
 * @brief Processes the given assembly file.
 *
 * This function checks the validity of the given filepath, constructs the full source path,
 * and processes the assembly file by reading its content and handling macro definitions.
 * It also handles memory allocation for paths and ensures proper cleanup.
 *
 * @param filepath Path to the file.
 * @param mcro_table Pointer to the macro table.
 * @return TRUE (1) if processing is successful, FALSE (0) otherwise.
 */
int process_file(const char* filepath, McroTable *mcro_table);


#endif /* PREPROCESSOR_H */