/* preprocessor_utils.h */
#ifndef PREPROCESSOR_UTILS_H
#define PREPROCESSOR_UTILS_H

#include <stdio.h>
#include "globals.h"
#include "errors.h"
#include "preprocessor.h"

/**
 * @brief Initializes the macro table to an empty state.
 *
 * This function initializes the macro table by setting the count to zero and
 * initializing the line count of each macro entry to zero.
 *
 * @param table Pointer to the macro table to initialize.
 */
void init_mcro_table(McroTable *table);

/**
 * @brief Validates if a given name is a legal macro name.
 *
 * This function checks if the provided name is a valid macro name by comparing it against
 * a list of reserved words. It trims any newline characters from the name before validation.
 *
 * @param name The name to validate.
 * @return ErrorCode ERROR_SUCCESS if the name is valid, otherwise an appropriate error code.
 */
ErrorCode is_valid_mcro_name(const char *name);

/**
 * @brief Adds a new macro to the macro table.
 *
 * If the macro name is valid and not a duplicate, it will be added to the macro table.
 *
 * @param table Pointer to the macro table.
 * @param name The name of the macro to add.
 * @return ERROR_SUCCESS if successful, error code otherwise.
 */
ErrorCode add_mcro(McroTable *table, const char *name);

/**
 * @brief Adds a line of content to the most recently added macro.
 *
 * This function appends a line to the last defined macro in the table. It ensures that the macro
 * table is not empty and that the macro does not exceed the maximum allowed lines.
 *
 * @param table Pointer to the macro table.
 * @param line The line to add to the macro.
 * @return ERROR_SUCCESS if successful, error code otherwise.
 */
ErrorCode add_line_to_mcro(McroTable *table, const char *line);

/**
 * @brief Processes the content as it would appear in the .am file and writes it to the target file.
 *
 * This function reads the source file line by line, expands macros when called, and removes macro
 * declarations and calls from the output. It creates a new file with the .am extension and writes
 * the processed content to it.
 *
 * @param source_fp Pointer to the source file.
 * @param source_filepath Path to the source file.
 * @param mcro_table Pointer to the macro table containing defined macros.
 * @param is_valid Pointer to the is_valid file flag.
 * @return TRUE (1) if processing is successful, FALSE (0) otherwise.
 */
int expand_macros_to_am_file (FILE *source_fp, const char *source_filepath, const McroTable *mcro_table, int *is_valid);

#endif /* PREPROCESSOR_UTILS_H */
