/* Header_Files/mcro.h */
#ifndef MCRO_H
#define MCRO_H

#include <stdio.h>

/* Maximum length of a macro name */
#define MAX_MCRO_NAME 31
/* Maximum number of lines in a macro definition */
#define MAX_MCRO_LINES 100
/* Maximum length of a macro line */
#define MAX_MCRO_LINE_LENGTH 81

/* Structure to store a macro definition */
typedef struct {
    char name[MAX_MCRO_NAME];
    char lines[MAX_MCRO_LINES][MAX_MCRO_LINE_LENGTH];
    int line_count;
} Mcro;

/* Structure to store all macros */
typedef struct {
    Mcro* mcros;
    int count;
} McroTable;

/* Process source file and create .am file with expanded macros */
int process_mcro(FILE* source_fp, const char* output_filename);

/* Initialize macro table */
McroTable* init_mcro_table(void);

/* Free macro table memory */
void free_mcro_table(McroTable* table);

/* Check if a name is a valid macro name */
int is_valid_mcro_name(const char* name);

/* Add a macro definition to the table */
int add_mcro(McroTable* table, const char* name);

/* Add a line to the current macro definition */
int add_mcro_line(McroTable* table, const char* line);

/* Find a macro by name in the table */
Mcro* find_mcro(McroTable* table, const char* name);

#endif /* MCRO_H */