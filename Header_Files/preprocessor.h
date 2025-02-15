/* preprocessor.h */
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "errors.h"
#include "globals.h"

/**
 * @struct Mcro
 * @brief Represents a macro with its name and content lines.
 */
typedef struct {
    char name[MAX_MCRO_NAME];
    char content[MAX_MCRO_LINES][MAX_LINE_LENGTH];
    int line_count;
} Mcro;

/**
 * @struct McroTable
 * @brief Holds all the macros defined in the source file.
 */
typedef struct {
    Mcro mcros[MAX_MCROS];
    int count;
} McroTable;

/**
 * @brief Processes an assembly file to identify and store macro definitions.
 *
 * @param fp File pointer to the assembly source file.
 * @param file_path The path to the source file.
 */
void process_as_file(FILE *fp, const char *file_path);

/**
 * @brief Processes the given assembly file.
 *
 * @param filepath Path to the file.
 * @return 1 if processing is successful, 0 otherwise.
 */
int process_file(const char *filepath);

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