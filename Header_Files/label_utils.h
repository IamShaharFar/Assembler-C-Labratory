/*
 * File: label_utils.h
 * Description: Header file for label utility functions in an assembler.
 */

 #ifndef LABEL_UTILS_H
 #define LABEL_UTILS_H
 
 #include "../Header_Files/globals.h"
 #include "../Header_Files/utils.h"
 #include "../Header_Files/errors.h"
 #include "../Header_Files/structs.h"
 
/*
 * Function: is_valid_label
 * ------------------------
 * Validates whether a given string is a valid label name.
 *
 * label: The label string to check.
 *
 * returns: ERROR_SUCCESS if the label is valid, otherwise an appropriate error code.
 */
ErrorCode is_valid_label(const char *label);
 
 /*
  * Function: label_exists
  * ----------------------
  * Checks whether a label exists in the label table.
  *
  * name: The label name to check.
  * label_table: The table of defined labels.
  *
  * returns: TRUE if the label exists, FALSE otherwise.
  */
 int label_exists(const char *name, LabelTable *label_table);
 
 /*
  * Function: is_valid_extern_label
  * -------------------------------
  * Validates an ".extern" directive and extracts the label.
  *
  * line: The input line containing the directive.
  *
  * returns: ERROR_SUCCESS if the label is valid, otherwise an appropriate error code.
  */
 ErrorCode is_valid_extern_label(const char *line);

 /**
 * @brief Checks if a given line is a valid .entry directive.
 *
 * This function verifies whether a .entry directive has a label that exists in the label table.
 *
 * @param line The line containing the .entry directive.
 * @param label_table Pointer to the label table to check.
 * @return ERROR_SUCCESS if the label exists, otherwise returns an appropriate error code.
 */
ErrorCode is_valid_entry_label(const char *line, LabelTable *label_table);

 /**
 * @brief Adds a new label to the label table.
 *
 * This function adds a label if it doesn't already exist. If the label exists,
 * it triggers an error.
 *
 * @param name The name of the label.
 * @param line_number The line number where the label was found.
 * @param line The full line content after the label.
 * @param label_table Pointer to the label table.
 * @return ERROR_SUCCESS if the label is added successfully, otherwise returns an appropriate error code.
 */
ErrorCode add_label(const char *name, int line_number, const char *line, const char *type, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table);
 
/**
 * @brief Prints the names of all macros in the given McroTable.
 *
 * @param mcro_table Pointer to the McroTable containing the macros.
 */
void print_mcro_names(const McroTable *mcro_table);

 #endif /* LABEL_UTILS_H */
 