/*
 * File: second_pass.h
 * Description: Header file for the second pass of the assembler.
 */

 #ifndef SECOND_PASS_H
 #define SECOND_PASS_H
 
 #include <stdio.h>   /* Required for FILE */
 #include <string.h>  /* Required for strncmp */
 #include <ctype.h>   /* Required for isspace */
 #include "globals.h" /* Ensure it contains necessary constants */
 #include "utils.h"   /* Ensure it has advance_to_next_token */
 
/**
 * @file second_pass.c
 * @brief Implements the second pass of the assembler.
 *
 * This function processes an assembly source file (.am) in its second pass,
 * resolving label addresses, handling entry directives, and updating the virtual PC.
 * It performs the following operations:
 * 
 * - Skips lines containing `.extern`, `.string`, or `.data` directives.
 * - Validates `.entry` directives and updates label metadata accordingly.
 * - Resolves label addresses, modifying VirtualPC storage as needed.
 * - Detects and reports errors such as undefined labels.
 * 
 * If any errors are encountered, the function sets `is_valid_file` to FALSE.
 *
 * @param am_file Pointer to the opened .am file for reading.
 * @param label_table Pointer to the LabelTable structure containing symbol information.
 * @param vpc Pointer to the VirtualPC structure storing assembled machine code.
 * 
 * @return int Returns TRUE if the file was processed successfully, FALSE if errors occurred.
 */
 int second_pass(FILE *am_file, LabelTable *label_table, VirtualPC *vpc);
 
 #endif /* SECOND_PASS_H */
 