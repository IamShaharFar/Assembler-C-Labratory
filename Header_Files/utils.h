/* Header_Files/utils.h */
#ifndef UTILS_H
#define UTILS_H

#define REGISTER_PREFIX "r"

/* Advances pointer past whitespace characters */
char* advance_to_next_token(char* str);

/* Validates register operand format (r0-r7) */
int validate_register_operand(const char* str);

/* Checks if line length exceeds MAX_LINE_LENGTH */
int check_line_length_exceeded(const char* line);

/* Detects multiple commas between tokens */
int detect_consecutive_commas(const char* str);

/* Ensures no extra tokens after command */
int verify_command_end(const char* str);

void trim_newline(char *str);

#endif /* UTILS_H */