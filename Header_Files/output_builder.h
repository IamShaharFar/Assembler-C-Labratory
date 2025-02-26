#ifndef OUTPUT_BUILDER_H
#define OUTPUT_BUILDER_H

#include "structs.h"

/**
 * @brief Writes the assembled machine code into a .ob file.
 *
 * Generates a .ob file containing the compiled machine code.
 * The first line contains instruction count (IC - 100) and data count (DC).
 * The subsequent lines list memory addresses and their corresponding machine code in hexadecimal.
 *
 * @param vpc Pointer to the VirtualPC structure containing the machine code.
 * @param filename The name of the .am file (without extension).
 */
void generate_object_file(const VirtualPC *vpc, const char *filename);

/**
 * @brief Comparison function for sorting labels by address.
 */
int compare_labels_by_address(const void *a, const void *b);

/**
 * @brief Writes the entry labels into a .ent file.
 *
 * This function creates a .ent file in the same directory as the .am file.
 * It writes labels marked with "entry" in increasing order of their addresses.
 * Each line contains the label's address (padded to 7 digits) and its name.
 *
 * @param label_table Pointer to the LabelTable structure containing label data.
 * @param filename The name of the .am file (without extension).
 */
void generate_entry_file(const LabelTable *label_table, const char *filename);

/**
 * @brief Writes the external labels into a .ext file.
 *
 * This function creates a .ext file in the same directory as the .am file.
 * It writes labels that have an address of 0 (indicating external references).
 * Each line contains the label's name (encoded string) and the memory address
 * where it is used (formatted as a 7-digit number).
 *
 * @param vpc Pointer to the VirtualPC structure containing machine code.
 * @param label_table Pointer to the LabelTable structure containing labels.
 * @param filename The name of the .am file (without extension).
 */
void generate_externals_file(const VirtualPC *vpc, const LabelTable *label_table, const char *filename);

#endif /* OUTPUT_BUILDER_H */
