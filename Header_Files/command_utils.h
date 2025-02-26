/*
 * File: command_utils.h
 * Description: Header file for command validation and processing functions in an assembler.
 */

 #ifndef COMMAND_UTILS_H
 #define COMMAND_UTILS_H
 
 #include "../Header_Files/errors.h"
 #include "../Header_Files/globals.h"
 
 /*
  * Function: is_valid_command_name
  * -------------------------------
  * Checks if a given token is a valid command name.
  *
  * token: The token to check.
  *
  * returns: TRUE if the token is a valid command name, FALSE otherwise.
  */
 int is_valid_command_name(const char *token);
 
 /*
  * Function: is_valid_command
  * --------------------------
  * Validates a given assembly command.
  *
  * line: The input command line.
  *
  * returns: ERROR_SUCCESS if the command is valid, otherwise an appropriate error code.
  */
 ErrorCode is_valid_command(const char *line);
 
 /*
  * Function: get_expected_params
  * -----------------------------
  * Determines the number of parameters expected for a given command.
  *
  * command_name: The name of the command.
  *
  * returns: The expected number of parameters (0, 1, or 2), or -1 if invalid.
  */
 int get_expected_params(const char *command_name);
 
 /*
  * Function: skip_command_name
  * ---------------------------
  * Moves the pointer past the command name in a given line.
  *
  * line: The input line.
  *
  * returns: Pointer to the next non-space character after the command.
  */
 char *skip_command_name(char *line);
 
 /*
  * Function: validate_parameters
  * -----------------------------
  * Validates the parameters of a command based on the expected count.
  *
  * line: The input line containing parameters.
  * expected_params: The expected number of parameters.
  * params: Array to store extracted parameters.
  *
  * returns: ERROR_SUCCESS if parameters are valid, otherwise an appropriate error code.
  */
 ErrorCode validate_parameters(char *line, int expected_params, char params[][MAX_LINE_LENGTH]);
 
 /*
  * Function: validate_command_params
  * ---------------------------------
  * Validates the specific parameters for a given command.
  *
  * command_name: The name of the command.
  * params: The extracted parameters.
  *
  * returns: ERROR_SUCCESS if parameters are valid, otherwise an appropriate error code.
  */
 ErrorCode validate_command_params(const char *command_name, char params[2][MAX_LINE_LENGTH]);
 
 #endif /* COMMAND_UTILS_H */
 