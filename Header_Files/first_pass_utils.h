/* first_pass_utils.h */
#ifndef FIRST_PASS_UTILS_H
#define FIRST_PASS_UTILS_H

#include <stdio.h>
#include "globals.h"
#include "first_pass.h"
#include "structs.h"


/**
 * @brief Checks if a given token is a valid command or reserved word.
 *
 * This function compares the provided token against the list of reserved words.
 * If the token matches any reserved word, it returns TRUE; otherwise, FALSE.
 *
 * @param token The string to check.
 * @return TRUE if the token is a reserved word, FALSE otherwise.
 */
int is_valid_command_name(const char *token);

/**
 * @brief Checks if a given line is a data storage instruction (.data or .string).
 *
 * This function inspects the line to see if it starts with .data or .string.
 *
 * @param line The line to check.
 * @return TRUE if the line contains a data storage instruction, FALSE otherwise.
 */
ErrorCode is_data_storage_instruction(const char *line);

int is_valid_entry_or_extern_line(char *line);

/**
 * @brief Checks if a given label is valid.
 *
 * A valid label starts with a letter, does not match any reserved word,
 * and is not a register name (r0 to r7).
 *
 * @param label The label to check.
 * @return TRUE if the label is valid, FALSE otherwise.
 */
int is_valid_label(const char *label);

/**
 * @brief Checks if a label already exists in the label table.
 *
 * @param name The name of the label to check.
 * @return 1 if the label exists, 0 otherwise.
 */
int label_exists(const char *name, LabelTable *label_table);

/**
 * @brief Validates an assembly command line for correct syntax and parameters.
 *
 * This function checks if a given command line follows the correct syntax rules:
 * 1. Removes leading spaces.
 * 2. Validates if the command name is a recognized command.
 * 3. Checks the expected number of parameters for the command.
 * 4. Verifies the correct placement of commas and parameters.
 *
 * @param line The command line to validate.
 * @return ERROR_SUCCESS if the command is valid, otherwise returns the appropriate error code.
 */
ErrorCode is_valid_command(const char *line);

/**
 * @brief Determines the expected number of parameters for a given command.
 *
 * @param command_index The index of the command in the command names array.
 * @return The number of expected parameters.
 */
int get_expected_params(const char *command_name);

/**
 * @brief Skips over the command name to the next part of the line.
 *
 * @param line The command line.
 * @return Pointer to the part of the line after the command name.
 */
char *skip_command_name(char *line);

/**
 * @brief Validates the parameters of a command based on expected count.
 *
 * @param line The part of the command line containing parameters.
 * @param expected_params The expected number of parameters.
 * @return ERROR_SUCCESS if parameters are valid, otherwise returns the appropriate error code.
 */
ErrorCode validate_parameters(char *line, int expected_params, char params[][MAX_LINE_LENGTH]);

ErrorCode validate_command_params(const char *command_name, char params[2][MAX_LINE_LENGTH]);

int count_data_or_string_elements(char *ptr);

#endif /* FIRST_PASS_UTILS_H */