#ifndef GLOBALS_H
#define GLOBALS_H

#define INT_MAX 8388607
#define INT_MIN -8388608


#define MAX_LINE_LENGTH 81
#define MAX_FILENAME_LENGTH 30

#define MAX_MCRO_NAME_LENGTH 31
#define MAX_LABEL_LENGTH 31
#define MAX_MCROS 50
#define MAX_MCRO_LINES 100

#define MAX_LABELS 100

#define TRUE 1
#define FALSE 0

/* ANSI Color Codes */
#define COLOR_RED "\033[31m"      
#define COLOR_YELLOW "\033[33m"  
#define COLOR_RESET "\033[0m"  


extern const char *reserved_words[];
#define RESERVED_WORDS_COUNT 28 

extern const char *commands_names[];
#define RESERVED_COMMANDS_COUNT 16 

#include "../Header_Files/structs.h" 

extern const CommandInfo commands_info[RESERVED_COMMANDS_COUNT]; /* define array size */

#endif /* GLOBALS_H */
