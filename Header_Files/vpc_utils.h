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

int print_data_or_string_binary(char *ptr, int address, VirtualPC *vpc);

void int_to_binary_24(int num, char binary_str[25]);

int generate_binary_command(const char *line, int address, VirtualPC *vpc);

void print_binary(unsigned int num, int bits);

void print_virtual_pc_memory(const VirtualPC *vpc);

/**
 * @brief Prints words where the encoded string starts with '&' or contains a label from the label table.
 *
 * This function scans the VirtualPC storage from address 100 to (100 + IC + DC),
 * checking if a word's `command_str` field starts with '&' (indicating relative addressing)
 * or contains a label from the label table.
 *
 * @param vpc Pointer to the VirtualPC structure.
 * @param label_table Pointer to the LabelTable structure.
 */
void print_words_with_labels(VirtualPC *vpc, const LabelTable *label_table);

#endif /* VPC_UTILS_H */