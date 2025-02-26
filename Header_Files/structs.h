#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>


/* Constants */
#define STORAGE_SIZE (1 << 21)  /* 2^21 slots */
#define WORD_SIZE 24            /* Each word is 24 bits */

/* Data structure for a single word */
typedef struct {
    int32_t value : WORD_SIZE;  /* 24-bit signed integer (2's complement) */
    char encoded[31]; 
} Word;

/* Data structure for the virtual PC */
typedef struct {
    Word storage[STORAGE_SIZE];  /* Storage with 2^21 words */
    uint32_t IC;       /* Current address of command */
    uint32_t DC;       /* Counter for data storage */
} VirtualPC;

/* Define CommandInfo struct */
typedef struct {
    const char *name;
    int opcode;
    int funct;
} CommandInfo;

/**
 * @struct Mcro
 * @brief Represents a macro with its name and content lines.
 */
typedef struct {
    char name[31];
    char content[100][81];
    int line_count;
} Mcro;

/**
 * @struct McroTable
 * @brief Holds all the macros defined in the source file.
 */
typedef struct {
    Mcro mcros[50];
    int count;
} McroTable;

/**
 * @struct Label
 * @brief Represents a label with its name, line number, and the full line content.
 */
typedef struct {
    char name[81];
    int line_number;
    char line[81];
    unsigned int address; /* Address should start from 100 */
    char type[81]; /* Data or Code */
} Label;

/**
 * @struct LabelTable
 * @brief Holds a list of labels found in the source file.
 */
typedef struct {
    Label labels[100];
    int count;
} LabelTable;

#endif /* STRUCTS_H */
