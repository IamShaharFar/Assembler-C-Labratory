#ifndef GLOBALS_H
#define GLOBALS_H

#include "../Header_Files/structs.h"  /* Ensure CommandInfo is defined before use */

#define MAX_LINE_LENGTH 81
#define MAX_FILENAME_LENGTH 30

#define MAX_MCRO_NAME 31
#define MAX_MCROS 50
#define MAX_MCRO_LINES 100

#define MAX_LABELS 100

#define TRUE 1
#define FALSE 0

extern const char *reserved_words[];
#define RESERVED_WORDS_COUNT 27 

extern const char *commands_names[];
#define RESERVED_COMMANDS_COUNT 16 

extern const CommandInfo commands_info[RESERVED_COMMANDS_COUNT]; /* Explicitly define array size */

#endif /* GLOBALS_H */
