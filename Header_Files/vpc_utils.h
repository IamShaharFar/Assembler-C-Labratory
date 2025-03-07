/* vpc_utils.h */
#ifndef VPC_UTILS_H
#define VPC_UTILS_H

#include "structs.h"


/**
 * @brief Processes a .data or .string directive and stores the values in the VirtualPC storage.
 *
 * This function processes a directive from the input string pointed to by `ptr`. It can handle
 * either a .data directive, which contains a list of numbers, or a .string directive, which 
 * contains a string of characters. The function stores the processed values in the VirtualPC 
 * storage, ensuring that only the lower 24 bits of each value are stored.
 *
 * @param ptr Pointer to the input string containing the directive.
 * @param vpc Pointer to the VirtualPC structure where the values will be stored.
 * @return The count of processed items (numbers or characters).
 */
int process_data_or_string_directive(char *ptr, VirtualPC *vpc);

/**
 * @brief Generates words from a command from a valid line of command and stores it in the VirtualPC storage.
 *
 * This function processes a valid line of command from the input string `line`. It extracts the command
 * and its parameters, determines the opcode and function code, and generates the corresponding binary
 * representation. The command and its parameters are then stored in the VirtualPC storage.
 *
 * @param line Pointer to the input string containing the command line.
 * @param vpc Pointer to the VirtualPC structure where the binary command will be stored.
 * @return The count of stored items (1 for the command itself, plus any additional parameters).
 */
int process_and_store_command(const char *line, VirtualPC *vpc);

void process_operand(const char *param, unsigned int *first_word, unsigned int *word, int shift_opcode, int shift_reg, int *param_flag);


/**
 * @brief Resolves and updates words in VirtualPC storage with label addresses.
 *
 * This function scans the VirtualPC storage from address 100 to (IC + DC),
 * checking if a word's `encoded` field starts with '&' (indicating relative addressing)
 * or contains a label from the label table. It then updates the word's value with the
 * appropriate address or relative offset.
 *
 * @param vpc Pointer to the VirtualPC structure.
 * @param label_table Pointer to the LabelTable structure.
 */
int resolve_and_update_labels(VirtualPC *vpc, const LabelTable *label_table);

#endif /* VPC_UTILS_H */