/*
 * File: first_pass_utils.c
 * Description: Utility functions for handling data storage instructions in the first pass of assembly processing.
 */

 #include <string.h>  /* Required for strncmp */
 #include <stdlib.h>  /* Required for strtol */
 #include <ctype.h>   /* Required for isspace */
 #include "../Header_Files/first_pass_utils.h"
 #include "../Header_Files/label_utils.h"
 #include "../Header_Files/first_pass.h"
 #include "../Header_Files/globals.h"
 #include "../Header_Files/utils.h"
 #include "../Header_Files/errors.h"
 
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
 ErrorCode is_data_storage_instruction(const char *line)
 {
     if (line == NULL || *line == '\0')
     {
         return ERROR_INVALID_STORAGE_DIRECTIVE;
     }
 
     /* Skip leading spaces */
     while (isspace((unsigned char)*line))
     {
         line++;
     }
 
     /* Check for .data directive */
     if (strncmp(line, ".data", 5) == 0 && isspace((unsigned char)line[5]))
     {
         line += 5;
         while (isspace((unsigned char)*line))
             line++;
 
         if (*line == '\0')
         {
             return ERROR_INVALID_DATA_NO_NUMBER;
         }
 
         while (*line)
         {
             char *endptr;
             strtol(line, &endptr, 10); /* Convert number */
             if (endptr == line)
             {
                 return ERROR_INVALID_DATA_NON_NUMERIC;
             }
 
             line = endptr;
             while (isspace((unsigned char)*line))
                 line++;
 
             /* Handle commas between numbers */
             if (*line == ',')
             {
                 line++;
                 while (isspace((unsigned char)*line))
                     line++;
                 if (*line == '\0')
                 {
                     return ERROR_INVALID_DATA_TRAILING_COMMA;
                 }
             }
             else if (*line != '\0')
             {
                 return ERROR_INVALID_DATA_UNEXPECTED_CHAR;
             }
         }
         return ERROR_SUCCESS;
     }
     /* Check for .string directive */
     else if (strncmp(line, ".string", 7) == 0 && isspace((unsigned char)line[7]))
     {
         line += 7;
         while (isspace((unsigned char)*line))
             line++;
 
         if (*line != '"')
         {
             return ERROR_INVALID_STRING_NO_QUOTE;
         }
         line++;
 
         /* Iterate over string content */
         while (*line && *line != '"')
         {
             line++;
         }
 
         if (*line != '"')
         {
             return ERROR_INVALID_STRING_MISSING_END_QUOTE;
         }
         line++;
 
         while (isspace((unsigned char)*line))
             line++;
         if (*line != '\0')
         {
             return ERROR_INVALID_STRING_EXTRA_CHARS;
         }
         return ERROR_SUCCESS;
     }
 
     return ERROR_INVALID_STORAGE_DIRECTIVE;
 }
 
 /*
  * Function: count_data_or_string_elements
  * ---------------------------------------
  * Counts elements in a .data or .string directive.
  *
  * ptr: Pointer to the directive content.
  *
  * returns: Number of elements (integers for .data, characters for .string).
  */
 int count_data_or_string_elements(char *ptr)
 {
     int count = 0;
 
     /* Check for .data directive */
     if (strncmp(ptr, ".data", 5) == 0)
     {
         ptr += 5;
         ptr = advance_to_next_token(ptr);
 
         /* Iterate over the numbers in the .data directive */
         while (*ptr)
         {
             strtol(ptr, &ptr, 10); /* Convert number */
             count++;
             ptr = advance_to_next_token(ptr);
             if (*ptr == ',')
             {
                 ptr++;
                 ptr = advance_to_next_token(ptr);
             }
         }
     }
     /* Check for .string directive */
     else if (strncmp(ptr, ".string", 7) == 0)
     {
         ptr += 7;
         ptr = advance_to_next_token(ptr);
 
         if (*ptr == '"')
         {
             ptr++;
             while (*ptr && *ptr != '"')
             {
                 count++; /* Count each character inside the string */
                 ptr++;
             }
         }
     }
     return count;
 }
 