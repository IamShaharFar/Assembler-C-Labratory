/* preprocessor.h */
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "errors.h"

#define MAX_MCRO_NAME 31
#define MAX_MCROS 100
#define MAX_MCRO_LINES 100
#define MAX_LINE_LENGTH 81

typedef struct {
    char name[MAX_MCRO_NAME];
    char content[MAX_MCRO_LINES][MAX_LINE_LENGTH];
    int line_count;
} Mcro;

typedef struct {
    Mcro mcros[MAX_MCROS];
    int count;
} McroTable;

void process_as_file(FILE* fp);
void init_mcro_table(McroTable* table);
ErrorCode add_mcro(McroTable* table, const char* name);
void print_mcro_table(const McroTable* table);
ErrorCode add_line_to_mcro(McroTable* table, const char* line);

#endif