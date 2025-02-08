/* preprocessor_utils.h */
#ifndef PREPROCESSOR_UTILS_H
#define PREPROCESSOR_UTILS_H

#include <stdio.h>
#include "globals.h"
#include "errors.h"
#include "preprocessor.h"

/* Reserved words used in assembly language */
extern const char *reserved_words[];
#define RESERVED_WORDS_COUNT (sizeof(reserved_words) / sizeof(reserved_words[0]))

/**
 * @brief Initializes the macro table to an empty state.
 * @param table Pointer to the macro table to initialize.
 */
void init_mcro_table(McroTable *table);

/**
 * @brief Validates if a given name is a legal macro name.
 * @param name The name to validate.
 * @return TRUE if the name is valid, FALSE otherwise.
 */
int is_valid_mcro_name(const char *name);

/**
 * @brief Adds a new macro to the macro table.
 * @param table Pointer to the macro table.
 * @param name The name of the macro to add.
 * @return ERROR_SUCCESS if successful, error code otherwise.
 */
ErrorCode add_mcro(McroTable *table, const char *name);

/**
 * @brief Adds a line of content to the most recently added macro.
 * @param table Pointer to the macro table.
 * @param line The line to add to the macro.
 * @return ERROR_SUCCESS if successful, error code otherwise.
 */
ErrorCode add_line_to_mcro(McroTable *table, const char *line);

/**
 * @brief Prints all macros stored in the macro table.
 * @param table Pointer to the macro table to print.
 */
void print_mcro_table(const McroTable *table);

/**
 * @brief Processes the content as it would appear in the .am file and prints it to stdout.
 * 
 * This function simulates the .am file content by processing the source file. It expands macros
 * when called and removes macro declarations and calls from the output.
 * 
 * @param source_fp Pointer to the source file.
 * @param mcro_table Pointer to the macro table containing defined macros.
 * @return TRUE if processing is successful, FALSE otherwise.
 */
int create_am_file(FILE *source_fp, const char *source_filepath, const McroTable *mcro_table);

#endif /* PREPROCESSOR_UTILS_H */
