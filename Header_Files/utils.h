/* Header_Files/utils.h */
#ifndef UTILS_H
#define UTILS_H

#define REGISTER_PREFIX "r"

#include "../Header_Files/structs.h"

/* Advances pointer past whitespace characters */
char* advance_to_next_token(char* str);

char* advance_past_token(char* str);

/**
 * @brief Advances the pointer past the current token until it hits a space, tab, or comma.
 *
 * @param str The input string starting from the current token.
 * @return Pointer to the first space, tab, or comma after the token.
 */
char* advance_past_token_or_comma(char* str);

/* Validates register operand format (r0-r7) */
int validate_register_operand(const char* str);

/* Checks if line length exceeds MAX_LINE_LENGTH */
int check_line_length_exceeded(const char* line);

/* Detects multiple commas between tokens */
int detect_consecutive_commas(const char* str);

/* Ensures no extra tokens after command */
int verify_command_end(const char* str);

void trim_newline(char *str);

void init_label_table(LabelTable *label_table);

void init_virtual_pc(VirtualPC *vpc);

#endif /* UTILS_H */