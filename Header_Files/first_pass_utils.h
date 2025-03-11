/* first_pass_utils.h */
#ifndef FIRST_PASS_UTILS_H
#define FIRST_PASS_UTILS_H

#include <stdio.h>
#include "globals.h"
#include "first_pass.h"
#include "structs.h"


/**
 * @brief Validates if a given line is a proper data storage directive (.data or .string) and checks its syntax correctness.
 *
 * The function checks for the presence and correctness of the directive syntax, including validation of numeric data
 * in the .data directive and string encapsulation in the .string directive. It identifies various formatting errors,
 * such as missing commas, consecutive commas, invalid numeric values, missing quotes, or extra characters.
 *
 * @param line  A pointer to a string containing the line to validate.
 *
 * @return      An ErrorCode indicating the validation result, ERROR_SUCCESS if valid, or a specific error code otherwise.
 */
ErrorCode is_data_storage_instruction(char *line);

/**
 * @brief the number of elements specified in a data (.data) or string (.string) directive.
 *
 * For .data directives, it counts each numeric value. For .string directives, it counts the characters
 * within the quotes. The function assumes the input pointer initially points to a valid directive.
 *
 * @param ptr  Pointer to the string containing the directive and its content.
 *
 * @return     int The number of data elements or characters in the directive.
 */
int count_data_or_string_elements(char *ptr);


#endif /* FIRST_PASS_UTILS_H */