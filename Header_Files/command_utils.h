/*
 * File: command_utils.h
 * Description: Header file for command validation and processing functions in an assembler.
 */

 #ifndef COMMAND_UTILS_H
 #define COMMAND_UTILS_H
 
 #include "../Header_Files/errors.h"
 #include "../Header_Files/globals.h"
 
/**
 * @brief checks if a token is a valid command name.
 *
 * compares the input token against the list of reserved command words.
 *
 * @param token the string to check
 * @return TRUE if the token is a reserved command, FALSE otherwise
 */
 int is_valid_command_name(const char *token);
 
/**
 * @brief validates a command line by checking command name and its parameters.
 *
 * extracts the command name, checks if it is known, and validates the number
 * and format of its parameters according to expected rules.
 *
 * @param line the full command line to validate
 * @return an ErrorCode indicating success or the specific validation failure
 */

 ErrorCode is_valid_command(const char *line);
 
/**
 * @brief returns the expected number of parameters for a command.
 *
 * checks the command name and returns 0, 1, or 2 based on its specification.
 * returns -1 if the command name is invalid or unrecognized.
 *
 * @param command_name the name of the command to check
 * @return number of expected parameters (0–2), or -1 if invalid
 */

 int get_expected_params(const char *command_name);
 
/**
 * @brief skips over the command name in a line and returns the next token.
 *
 * advances the pointer past the first word (command name), then skips any
 * following whitespace to return the start of the next meaningful token.
 *
 * @param line pointer to the input line
 * @return pointer to the next token after the command name
 */
 char *skip_command_name(char *line);
 
/**
 * @brief validates and extracts parameters from a command line.
 *
 * parses the line and checks if the number and formatting of parameters match
 * the expected count. detects missing, extra, or non-matching parameters.
 *
 * @param line the command line after the command name
 * @param expected_params number of parameters expected (0–2)
 * @param params output array to store the parsed parameters
 * @return ErrorCode indicating success or a specific validation error
 */
 ErrorCode validate_parameters(char *line, int expected_params, char params[][MAX_LINE_LENGTH]);
 
/**
 * @brief validates operand types for a given command.
 *
 * checks if the operands passed to a command match the expected addressing modes
 * for that command (e.g., register, immediate, direct, relative).
 *
 * @param command_name the name of the command to validate
 * @param params array of up to two parameters to check
 * @return ErrorCode indicating success or the specific operand error
 */
 ErrorCode validate_command_params(const char *command_name, char params[2][MAX_LINE_LENGTH]);
 
 #endif /* COMMAND_UTILS_H */
 