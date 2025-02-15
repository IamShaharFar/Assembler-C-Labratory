/* first_pass.h */
#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include "globals.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/errors.h"


/**
 * @struct Label
 * @brief Represents a label with its name, line number, and the full line content.
 */
typedef struct {
    char name[MAX_LINE_LENGTH];
    int line_number;
    char line[MAX_LINE_LENGTH];
} Label;

/**
 * @struct LabelTable
 * @brief Holds a list of labels found in the source file.
 */
typedef struct {
    Label labels[MAX_LABELS];
    int count;
} LabelTable;

/**
 * @brief Initializes the label table to an empty state.
 */
void init_label_table();

/**
 * @brief Processes the assembly file for the first pass to identify labels.
 *
 * This function reads through each line of the file and checks for label definitions.
 * Labels are identified as names followed by a colon (e.g., LABEL:).
 * The labels are stored in a LabelTable structure.
 *
 * @param fp Pointer to the assembly source file.
 */
void first_pass(FILE *fp, VirtualPC *vpc);

/**
 * @brief Prints all labels stored in the label table.
 */
void print_label_table();

/**
 * @brief Adds a new label to the label table.
 *
 * This function adds a label if it doesn't already exist. If the label exists,
 * it triggers an error.
 *
 * @param name The name of the label.
 * @param line_number The line number where the label was found.
 * @param line The full line content after the label.
 * @return ERROR_SUCCESS if the label is added successfully, otherwise returns an appropriate error code.
 */
ErrorCode add_label(const char *name, int line_number, const char *line);

#endif /* FIRST_PASS_H */