/* Header_Files/vpc_utils.h */
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
 * @param storage_full Pointer to an integer flag that will be set to if the storage is full.
 * @return The count of processed items (numbers or characters).
 */
int process_data_or_string_directive(char *ptr, VirtualPC *vpc, int *storage_full);

/**
 * @brief Generates words from a command from a valid line of command and stores it in the VirtualPC storage.
 *
 * This function processes a valid line of command from the input string `line`. It extracts the command
 * and its parameters, determines the opcode and function code, and generates the corresponding binary
 * representation. The command and its parameters are then stored in the VirtualPC storage.
 *
 * @param line Pointer to the input string containing the command line.
 * @param vpc Pointer to the VirtualPC structure where the binary command will be stored.
 * @param storage_full Pointer to an integer flag that will be set to if the storage is full.
 * @return The count of stored items (1 for the command itself, plus any additional parameters).
 */
int process_and_store_command(const char *line, VirtualPC *vpc, int *storage_full);

/**
 * @brief Processes an operand and updates the binary representation in the provided words.
 *
 * This function processes an operand from the input string `param`. It determines the addressing mode
 * (immediate, relative, direct, or register), updates the `first_word` and `word` with the appropriate
 * binary representation, and sets the `param_flag` if the operand is valid.
 *
 * @param param Pointer to the input string containing the operand.
 * @param first_word Pointer to the first word of the binary representation to be updated.
 * @param word Pointer to the word where the operand value will be stored.
 * @param shift_opcode Number of bits to shift the opcode for the addressing mode.
 * @param shift_reg Number of bits to shift the register number.
 * @param param_flag Pointer to the flag indicating if the operand is valid.
 */
void process_operand(const char *param, unsigned int *first_word, unsigned int *word, int shift_opcode, int shift_reg, int *param_flag);


#endif /* VPC_UTILS_H */