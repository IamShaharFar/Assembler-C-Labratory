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

void print_data_or_string_binary(char *ptr);

void int_to_binary_24(int num, char binary_str[25]);

void generate_binary_command(const char *line);

void print_binary(unsigned int num, int bits);

#endif /* VPC_UTILS_H */