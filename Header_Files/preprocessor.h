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
 * @param fp File pointer to the assembly source file.
 * @param file_path Path to the source file.
 * @param mcro_table Pointer to the macro table.
 * @return TRUE if the file is valid and processed successfully, FALSE otherwise.
 *
 * Example of an error: A file containing a line longer than the maximum allowed length.
 */
int process_as_file(FILE *fp, const char *file_path, McroTable *mcro_table);

/**
 * @brief Processes the given assembly file.
 *
 * @param filepath Path to the file.
 * @return 1 if processing is successful, 0 otherwise.
 */
int process_file(const char* filepath, McroTable *mcro_table);

/**
 * @brief Saves the processed content as a .am file.
 *
 * @param source_fp Pointer to the source file.
 * @param source_filepath The full path of the source file to create the corresponding .am file.
 * @param mcro_table Pointer to the macro table containing defined macros.
 * @return TRUE if processing is successful, FALSE otherwise.
 */
int create_am_file(FILE *source_fp, const char *source_filepath, const McroTable *mcro_table);

#endif /* PREPROCESSOR_H */