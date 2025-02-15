#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

/* Constants */
#define STORAGE_SIZE (1 << 21)  /* 2^21 slots */
#define WORD_SIZE 24            /* Each word is 24 bits */

/* Data structure for a single word */
typedef struct {
    int32_t value : WORD_SIZE;  /* 24-bit signed integer (2's complement) */
} Word;

/* Data structure for the virtual PC */
typedef struct {
    Word storage[STORAGE_SIZE];  /* Storage with 2^21 words */
    uint32_t IC;       /* Current address of command */
    uint32_t DC;       /* Counter for data storage */

} VirtualPC;

#endif /* STRUCTS_H */