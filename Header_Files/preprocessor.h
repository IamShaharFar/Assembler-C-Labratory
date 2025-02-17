/* preprocessor.h */
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "errors.h"
#include "globals.h"
#include "structs.h"

/**
 * @brief Processes an assembly file to identify and store macro definitions.
 *
 * @param fp File pointer to the assembly source file.
 * @param file_path The path to the source file.
 */
void process_as_file(FILE *fp, const char *file_path, McroTable *mcro_table);

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