/* first_pass.h */
#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include "globals.h"
#include "../Header_Files/structs.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/structs.h"

/**
 * @brief Performs the first pass on an assembly source file to identify and process labels, directives, and commands.
 *
 * This function reads the assembly file line-by-line, identifies labels, verifies their validity, processes data storage
 * directives (.data, .string), command instructions, and external label declarations (.extern). It updates the virtual PC 
 * and label table. Errors encountered during processing are printed.
 *
 * @param fp           Pointer to the assembly (.am) source file to process.
 * @param vpc          Pointer to the VirtualPC struct managing memory addresses.
 * @param label_table  Pointer to the LabelTable struct for storing identified labels.
 * @param mcro_table   Pointer to the McroTable struct containing macro definitions.
 *
 * @return             int TRUE if the file was processed without errors, FALSE otherwise.
 */
int first_pass(FILE *fp, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table);

#endif /* FIRST_PASS_H */