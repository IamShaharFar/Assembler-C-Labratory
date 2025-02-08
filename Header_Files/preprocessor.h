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

void print_mcro_if_called(FILE *fp);
void process_as_file(FILE *fp, const char *file_path);

#endif