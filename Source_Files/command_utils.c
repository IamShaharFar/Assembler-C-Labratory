/*
 * File: command_utils.c
 * Description: Utility functions for validating and processing assembly commands.
 */

 #include <stdio.h>   /* Required for sscanf */
 #include <stdlib.h>  /* Required for strtol */
 #include <string.h>  /* Required for strcmp, strncpy, strspn */
 #include <ctype.h>   /* Required for isspace */
 #include "../Header_Files/command_utils.h"
 #include "../Header_Files/label_utils.h"
 #include "../Header_Files/globals.h"
 #include "../Header_Files/utils.h"
 #include "../Header_Files/errors.h"
 
 /*
  * Function: is_valid_command_name
  * -------------------------------
  * Checks if a given token is a valid command name.
  *
  * token: The token to check.
  *
  * returns: TRUE if the token is a valid command name, FALSE otherwise.
  */
 int is_valid_command_name(const char *token)
 {
     size_t i;
     if (token == NULL || *token == '\0')
     {
         return FALSE;
     }
 
     /* Compare token with known reserved command words */
     for (i = 0; i < RESERVED_WORDS_COUNT; i++)
     {
         if (strcmp(token, reserved_words[i]) == 0)
         {
             return TRUE;
         }
     }
     return FALSE;
 }
 
 /*
  * Function: is_valid_command
  * --------------------------
  * Validates a given assembly command.
  *
  * line: The input command line.
  *
  * returns: ERROR_SUCCESS if the command is valid, otherwise an appropriate error code.
  */
 ErrorCode is_valid_command(const char *line)
 {
     char temp_line[MAX_LINE_LENGTH];
     char *ptr, *temp_ptr;
     char params[2][MAX_LINE_LENGTH];
     char command_name[MAX_LINE_LENGTH]; /* To store the command name */
     int expected_params;
     char saved_char;
     ErrorCode err;
    
     /* Copy the line to a temporary buffer */
     strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
     temp_line[MAX_LINE_LENGTH - 1] = '\0';
 
     /* Advance to the first non-space character */
     ptr = advance_to_next_token(temp_line);
 
     /* Extract the command name */
     temp_ptr = ptr;
     while (*temp_ptr && !isspace((unsigned char)*temp_ptr))
     {
         temp_ptr++;
     }
 
     /* Save the command name */
     strncpy(command_name, ptr, temp_ptr - ptr);
     command_name[temp_ptr - ptr] = '\0'; /* Null-terminate the command name */
 
     saved_char = *temp_ptr;
     *temp_ptr = '\0';
 
     /* Determine the expected number of parameters for this command */
     expected_params = get_expected_params(command_name);
     if (expected_params == -1)
     {
         return ERROR_UNKNOWN_COMMAND; /* Unknown command */
     }
 
     *temp_ptr = saved_char;
 
     /* Move pointer past the command name */
     ptr = skip_command_name(ptr);
     ptr = advance_to_next_token(ptr);
 
     /* Validate the parameters based on the expected count */
     err = validate_parameters(ptr, expected_params, params);
     if (err != ERROR_SUCCESS)
     {
         return err; /* Return specific error for parameter validation */
     }
     else
     {
         err = validate_command_params(command_name, params);
         if (err != ERROR_SUCCESS)
         {
             return err;
         }
     }
 
     return ERROR_SUCCESS; /* Command is valid */
 }
 
 /*
  * Function: get_expected_params
  * -----------------------------
  * Determines the number of parameters expected for a given command.
  *
  * command_name: The name of the command.
  *
  * returns: The expected number of parameters (0, 1, or 2), or -1 if invalid.
  */
 int get_expected_params(const char *command_name)
 {
     /* Commands expecting 2 parameters */
     if (strcmp(command_name, "mov") == 0 || strcmp(command_name, "cmp") == 0 ||
         strcmp(command_name, "add") == 0 || strcmp(command_name, "sub") == 0 ||
         strcmp(command_name, "lea") == 0)
     {
         return 2;
     }
 
     /* Commands expecting 1 parameter */
     if (strcmp(command_name, "clr") == 0 || strcmp(command_name, "not") == 0 ||
         strcmp(command_name, "inc") == 0 || strcmp(command_name, "dec") == 0 ||
         strcmp(command_name, "jmp") == 0 || strcmp(command_name, "bne") == 0 ||
         strcmp(command_name, "jsr") == 0 || strcmp(command_name, "red") == 0 ||
         strcmp(command_name, "prn") == 0)
     {
         return 1;
     }
 
     /* Commands expecting 0 parameters */
     if (strcmp(command_name, "rts") == 0 || strcmp(command_name, "stop") == 0)
     {
         return 0;
     }
 
     return -1; /* Invalid command name */
 }
 
 /*
  * Function: skip_command_name
  * ---------------------------
  * Moves the pointer past the command name in a given line.
  *
  * line: The input line.
  *
  * returns: Pointer to the next non-space character after the command.
  */
 char *skip_command_name(char *line)
 {
     while (*line && !isspace((unsigned char)*line))
     {
         line++;
     }
     return advance_to_next_token(line);
 }
 
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
 ErrorCode validate_parameters(char *line, int expected_params, char params[][MAX_LINE_LENGTH])
 {
     char *ptr = line;
     int i, j;
     if (expected_params == 0)
     {
         ptr = advance_to_next_token(ptr);
         return *ptr == '\0' ? ERROR_SUCCESS : ERROR_EXTRA_TEXT_AFTER_COMMAND; /* Ensure no extra characters */
     }
 
     /* Extract parameters based on the expected count */
     for (i = 0; i < expected_params; i++)
     {
         ptr = advance_to_next_token(ptr);
         if (*ptr == '\0')
             return ERROR_INVALID_PARAM_COUNT; /* Missing parameter */
 
         /* Extract parameter until whitespace or comma */
         j = 0;
         while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',')
         {
             params[i][j++] = *ptr++;
         }
         params[i][j] = '\0';
 
         /* Ensure valid parameter */
         if (j == 0 || strspn(params[i], " \t") == strlen(params[i]))
             return ERROR_INVALID_PARAM_COUNT;
 
         ptr = advance_to_next_token(ptr);
         if (i == 0 && expected_params == 2 && *ptr != ',')
             return ERROR_MISSING_COMMA; /* Missing comma */
 
         if (i == 0 && expected_params == 2)
             ptr++;
         ptr = advance_to_next_token(ptr);
         if (i == 0 && expected_params == 2 && *ptr == ',')
         {
             return ERROR_CONSECUTIVE_COMMAS; /* Consecutive commas */
         }
     }
 
     return *ptr == '\0' ? ERROR_SUCCESS : ERROR_EXTRA_TEXT_AFTER_COMMAND;
 }
 
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
 ErrorCode validate_command_params(const char *command_name, char params[2][MAX_LINE_LENGTH])
 {
     if (strcmp(command_name, "rts") == 0 || strcmp(command_name, "stop") == 0)
     {
         return ERROR_SUCCESS; /* No parameters required, already validated */
     }
 
     if (strcmp(command_name, "clr") == 0 || strcmp(command_name, "not") == 0 ||
         strcmp(command_name, "inc") == 0 || strcmp(command_name, "dec") == 0 ||
         strcmp(command_name, "red") == 0)
     {
         if (validate_register_operand(params[0]) || is_valid_label(params[0]) == ERROR_SUCCESS)
         {
             return ERROR_SUCCESS;
         }
         return ERROR_INVALID_DIRECT_OR_REGISTER_OPERAND;
     }
 
     if (strcmp(command_name, "mov") == 0 || strcmp(command_name, "add") == 0 || strcmp(command_name, "sub") == 0)
     {
         if (!(params[0][0] == '#' && strtol(params[0] + 1, NULL, 10) != 0) && is_valid_label(params[0]) != ERROR_SUCCESS && !validate_register_operand(params[0]))
         {
             return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND;
         }
         if (is_valid_label(params[1]) != ERROR_SUCCESS && !validate_register_operand(params[1]))
         {
             return ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND;
         }
         return ERROR_SUCCESS;
     }
 
     if (strcmp(command_name, "cmp") == 0)
     {
         if (!(params[0][0] == '#' && strtol(params[0] + 1, NULL, 10) != 0) && is_valid_label(params[0]) != ERROR_SUCCESS && !validate_register_operand(params[0]))
         {
             return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_FIRST_OPERAND;
         }
         if (!(params[1][0] == '#' && strtol(params[1] + 1, NULL, 10) != 0) && is_valid_label(params[1]) != ERROR_SUCCESS && !validate_register_operand(params[1]))
         {
             return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_SECOND_OPERAND;
         }
         return ERROR_SUCCESS;
     }
 
     if (strcmp(command_name, "lea") == 0)
     {
         if (is_valid_label(params[0]) != ERROR_SUCCESS)
         {
             return ERROR_INVALID_DIRECT_FIRST_OPERAND;
         }
         if (is_valid_label(params[1]) != ERROR_SUCCESS && !validate_register_operand(params[1]))
         {
             return ERROR_INVALID_DIRECT_OR_REGISTER_SECOND_OPERAND;
         }
         return ERROR_SUCCESS;
     }
 
     if (strcmp(command_name, "jmp") == 0 || strcmp(command_name, "bne") == 0 || strcmp(command_name, "jsr") == 0)
     {
         if (is_valid_label(params[0]) != ERROR_SUCCESS && !(params[0][0] == '&' && is_valid_label(params[0] + 1) == ERROR_SUCCESS))
         {
             return ERROR_INVALID_RELATIVE_OR_DIRECT_OPERAND;
         }
         return ERROR_SUCCESS;
     }
 
     if (strcmp(command_name, "prn") == 0)
     {
         if (params[0][0] == '#')
         {
             char *endptr;
             strtol(params[0] + 1, &endptr, 10);
             if (*endptr != '\0')
             {
                 return ERROR_INVALID_IMMEDIATE_OPERAND;
             }
             return ERROR_SUCCESS;
         }
         if (is_valid_label(params[0]) == ERROR_SUCCESS && !validate_register_operand(params[0]))
         {
             return ERROR_INVALID_IMMEDIATE_DIRECT_OR_REGISTER_OPERAND;
         }
         return ERROR_SUCCESS;
     }
 
     return ERROR_UNKNOWN_COMMAND; /* Invalid command */
 }
 