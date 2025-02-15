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

#endif /* VPC_UTILS_H */