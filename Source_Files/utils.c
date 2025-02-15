/* Source_Files/utils.c */
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../Header_Files/utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/structs.h"

/**
 * @brief Advances the pointer to the next non-whitespace character.
 *
 * This function moves the pointer forward, skipping over any leading 
 * whitespace characters (spaces, tabs, etc.) until it points to the 
 * first non-whitespace character. It does not modify the input string 
 * and is useful for locating the start of the next token in a line.
 *
 * Example:
 * Input: "   mov r1, r2"
 * Output: Pointer to 'm'
 *
 * @param str The input string to process.
 * @return Pointer to the first non-whitespace character in the string.
 */
char* advance_to_next_token(char* str) {
    while (str && *str && isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

/**
 * @brief Advances the pointer past the current token (non-space sequence).
 *
 * This function skips over the current token (a sequence of non-space characters)
 * and returns a pointer to the next whitespace or the end of the string.
 *
 * @param str The string pointer positioned at the start of the token.
 * @return Pointer to the character after the current token.
 */
char* advance_past_token(char* str) {
    while (str && *str && !isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

/**
 * @brief Advances the pointer past the current token until it hits a space, tab, or comma.
 *
 * @param str The input string starting from the current token.
 * @return Pointer to the first space, tab, or comma after the token.
 */
char* advance_past_token_or_comma(char* str) {
    while (*str && !isspace((unsigned char)*str) && *str != ',') {
        str++;
    }
    return str;
}


int validate_register_operand(const char* str)
{
    if (!str || strlen(str) != 2) 
    {
        return FALSE; /* Invalid length */
    }
    
    if (str[0] == 'r' && isdigit(str[1]) && str[1] >= '0' && str[1] <= '7')
    {
        return TRUE;
    }
    return FALSE;
}

int check_line_length_exceeded(const char* line) {
    return line && strlen(line) > MAX_LINE_LENGTH;
}

int detect_consecutive_commas(const char* str) {
    char* current_pos = (char*)str;
    
    while (current_pos && *current_pos) {
        if (*current_pos == ',') {
            current_pos = advance_to_next_token(current_pos + 1);
            if (*current_pos == ',') {
                return TRUE;
            }
        }
        current_pos++;
    }
    return FALSE;
}

int verify_command_end(const char* str) {
    str = advance_to_next_token((char*)str);
    return !str || !*str;
}

void trim_newline(char *str) {
    char *end;

    if (str == NULL || *str == '\0') return;

    end = str + strlen(str) - 1;
    while (end > str && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t')) {
        *end = '\0';
        end--;
    }
}

void init_virtual_pc(VirtualPC *vpc) {
    memset(vpc->storage, 0, sizeof(vpc->storage));  /* Initialize all storage to 0 */
    vpc->IC = 0;  /* Initialize IC to 0 */
    vpc->DC = 0;  /* Initialize DC to 0 */
}

