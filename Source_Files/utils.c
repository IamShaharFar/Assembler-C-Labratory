/* Source_Files/utils.c */
#include <string.h>
#include <ctype.h>
#include "../Header_Files/utils.h"
#include "../Header_Files/globals.h"

char* advance_to_next_token(char* str) {
    while (str && *str && isspace(*str)) {
        str++;
    }
    return str;
}

int validate_register_operand(const char* str) {
    /* Skip operation name and spaces */
    str = advance_to_next_token((char*)str);
    if (!str || !*str) return FALSE;
    
    /* Skip to register operand */
    while (*str && !isspace(*str)) str++;
    str = advance_to_next_token((char*)str);
    
    /* Validate register format r[0-7] */
    if (str && 
        strncmp(str, REGISTER_PREFIX, strlen(REGISTER_PREFIX)) == 0 && 
        isdigit(*(str + 1)) && 
        (*(str + 1) >= '0' && *(str + 1) <= '7') &&
        !isalnum(*(str + 2))) {
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