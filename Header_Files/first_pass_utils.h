/* first_pass_utils.h */
#ifndef FIRST_PASS_UTILS_H
#define FIRST_PASS_UTILS_H

#include <stdio.h>
#include "globals.h"
#include "first_pass.h"
#include "structs.h"


 /*
  * Function: is_data_storage_instruction
  * --------------------------------------
  * Checks if a given line is a valid .data or .string directive and validates its format.
  * Returns the appropriate error code if the directive is incorrect.
  *
  * line: The input line to validate.
  *
  * returns: ErrorCode based on validation outcome.
  */
ErrorCode is_data_storage_instruction(const char *line);

int count_data_or_string_elements(char *ptr);


#endif /* FIRST_PASS_UTILS_H */