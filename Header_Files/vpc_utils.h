/* vpc_utils.h */
#ifndef VPC_UTILS_H
#define VPC_UTILS_H

#include "structs.h"

/**
 * @brief Adds a word to the virtual PC at the current PC address.
 *
 * @param vpc Pointer to the VirtualPC structure.
 * @param word The Word to add.
 * @return int Returns 1 if successful, 0 if storage is full.
 */
int add_word_to_vpc(VirtualPC *vpc, Word word);

/**
 * @brief Processes .data or .string directive and stores binary words in VirtualPC.
 *
 * @param ptr Pointer to the content after .data or .string.
 * @return int The count of binary words stored.
 */
int process_data_or_string_directive(char *ptr, VirtualPC *vpc);

void int_to_binary_24(int num, char binary_str[25]);

int generate_binary_command(const char *line, VirtualPC *vpc);

void process_operand(const char *param, unsigned int *first_word, unsigned int *word, int shift_opcode, int shift_reg, int *param_flag);

void print_virtual_pc_memory(const VirtualPC *vpc);

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