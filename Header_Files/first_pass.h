/* first_pass.h */
#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include "globals.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/structs.h"

/**
 * @brief Processes the assembly file for the first pass to identify labels.
 *
 * This function reads through each line of the file and checks for label definitions.
 * Labels are identified as names followed by a colon (e.g., LABEL:).
 * The labels are stored in a LabelTable structure.
 *
 * @param fp Pointer to the assembly source file.
 * @param vpc Virtual PC pointer.
 * @param label_table Pointer to the label table.
 * @return int Returns TRUE if the file is valid, FALSE otherwise.
 */
int first_pass(FILE *fp, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table);

/**
 * @brief Prints all labels stored in the label table.
 */
/* void print_label_table(); */

void print_labels(const LabelTable *label_table);

#endif /* FIRST_PASS_H */