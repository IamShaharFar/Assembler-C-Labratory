#include "../Header_Files/globals.h"

/* Reserved words */
const char *reserved_words[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
    "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    ".data", ".string", ".entry", ".extern",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

/* Command names */
const char *commands_names[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
    "jmp", "bne", "jsr", "red", "prn", "rts", "stop"
};

/* Command info array with correct size */
const CommandInfo commands_info[RESERVED_COMMANDS_COUNT] = {
    {"mov", 0, -1}, 
    {"cmp", 1, -1},  
    {"add", 2, 1},   
    {"sub", 2, 2},  
    {"lea", 4, -1}, 
    {"clr", 5, 1},  
    {"not", 5, 2}, 
    {"inc", 5, 3},  
    {"dec", 5, 4},  
    {"jmp", 9, 1},  
    {"bne", 9, 2}, 
    {"jsr", 9, 3},  
    {"red", 12, -1},
    {"prn", 13, -1}, 
    {"rts", 14, -1},
    {"stop", 15, -1} 
};
