#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/globals.h"
#include "../Header_Files/mcro.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/utils.h"

static const char* reserved_words[] = {
    "mov", "cmp", "add", "sub", "not", "clr", "lea",
    "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
    "rts", "stop", ".data", ".string", ".entry", ".extern",
    NULL
};

McroTable* init_mcro_table(void) {
    McroTable* table = (McroTable*)malloc(sizeof(McroTable));
    if (!table) {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return NULL;
    }
    
    table->mcros = (Mcro*)malloc(sizeof(Mcro) * MAX_MCRO_LINES);
    if (!table->mcros) {
        free(table);
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return NULL;
    }
    
    table->count = 0;
    return table;
}

void free_mcro_table(McroTable* table) {
    if (table) {
        if (table->mcros) {
            free(table->mcros);
        }
        free(table);
    }
}

int is_valid_mcro_name(const char* name) {
    int i;
    
    if (!name || !*name) {
        return FALSE;
    }

    /* Check against reserved words */
    for (i = 0; reserved_words[i] != NULL; i++) {
        if (strcmp(name, reserved_words[i]) == 0) {
            return FALSE;
        }
    }

    /* First character must be a letter */
    if (!isalpha(*name)) {
        return FALSE;
    }

    /* Rest must be alphanumeric or underscore */
    for (i = 1; name[i] != '\0'; i++) {
        if (!isalnum(name[i]) && name[i] != '_') {
            return FALSE;
        }
    }

    return TRUE;
}

int add_mcro(McroTable* table, const char* name) {
    if (!table || !name) {
        return FALSE;
    }

    if (table->count >= MAX_MCRO_LINES) {
        return FALSE;
    }

    if (!is_valid_mcro_name(name)) {
        print_error_no_line(ERROR_MCRO_ILLEGAL_NAME);
        return FALSE;
    }

    if (find_mcro(table, name)) {
        print_error_no_line(ERROR_MCRO_DUPLICATE);
        return FALSE;
    }

    strncpy(table->mcros[table->count].name, name, MAX_MCRO_NAME - 1);
    table->mcros[table->count].name[MAX_MCRO_NAME - 1] = '\0';
    table->mcros[table->count].line_count = 0;
    table->count++;

    return TRUE;
}

int add_mcro_line(McroTable* table, const char* line) {
    Mcro* current_mcro;
    
    if (!table || !line || table->count == 0) {
        return FALSE;
    }

    current_mcro = &table->mcros[table->count - 1];
    
    if (current_mcro->line_count >= MAX_MCRO_LINES) {
        return FALSE;
    }

    strncpy(current_mcro->lines[current_mcro->line_count], 
            line, 
            MAX_MCRO_LINE_LENGTH - 1);
    current_mcro->lines[current_mcro->line_count][MAX_MCRO_LINE_LENGTH - 1] = '\0';
    current_mcro->line_count++;

    return TRUE;
}

Mcro* find_mcro(McroTable* table, const char* name) {
    int i;
    
    if (!table || !name) {
        return NULL;
    }

    for (i = 0; i < table->count; i++) {
        if (strcmp(table->mcros[i].name, name) == 0) {
            return &table->mcros[i];
        }
    }

    return NULL;
}

int process_mcro(FILE* source_fp, const char* output_filename) {
    FILE* output_fp;
    McroTable* table;
    char line[MAX_LINE_LENGTH];
    char mcro_name[MAX_MCRO_NAME];
    int in_mcro = FALSE;
    int has_error = FALSE;
    Mcro* current_mcro;
    char* token;
    int i;
    char original_line[MAX_LINE_LENGTH];
    char first_word[MAX_LINE_LENGTH];

    /* Validate input parameters */
    if (!source_fp || !output_filename) {
        print_error_no_line(ERROR_FILE_READ);
        return FALSE;
    }

    /* Initialize macro table */
    table = init_mcro_table();
    if (!table) {
        return FALSE;
    }

    /* Open output file */
    output_fp = fopen(output_filename, "w");
    if (!output_fp) {
        free_mcro_table(table);
        print_error_no_line(ERROR_FILE_WRITE);
        return FALSE;
    }

    /* Process file line by line */
    while (fgets(line, MAX_LINE_LENGTH, source_fp)) {
        char* trimmed_line = advance_to_next_token(line);
        
        /* Keep original line for non-macro content */
        strcpy(original_line, line);

        /* Handle empty or whitespace-only lines */
        if (!trimmed_line || !*trimmed_line || *trimmed_line == '\n') {
            if (!in_mcro) {
                fprintf(output_fp, "%s", original_line);
            }
            continue;
        }

        /* Get first word of the line */
        sscanf(trimmed_line, "%s", first_word);

        /* Check for macro definition start */
        if (strcmp(first_word, "mcro") == 0) {
            token = advance_to_next_token(trimmed_line + 4);
            if (!token || !*token) {
                print_error_no_line(ERROR_MCRO_NO_NAME);
                has_error = TRUE;
                break;
            }

            /* Extract macro name */
            sscanf(token, "%s", mcro_name);
            token = advance_to_next_token(token + strlen(mcro_name));
            
            /* Check for extra text after macro name */
            if (token && *token && !isspace(*token)) {
                print_error_no_line(ERROR_MCRO_EXTRA_TEXT);
                has_error = TRUE;
                break;
            }

            /* Add new macro to table */
            if (!add_mcro(table, mcro_name)) {
                has_error = TRUE;
                break;
            }
            in_mcro = TRUE;
            continue;
        }

        /* Check for macro definition end */
        if (strcmp(first_word, "mcroend") == 0) {
            in_mcro = FALSE;
            continue;
        }

        /* Handle lines inside macro definition */
        if (in_mcro) {
            if (!add_mcro_line(table, trimmed_line)) {
                has_error = TRUE;
                break;
            }
            continue;
        }

        /* Check if line is a macro call */
        current_mcro = find_mcro(table, first_word);
        if (current_mcro) {
            /* Expand macro by writing its content */
            for (i = 0; i < current_mcro->line_count; i++) {
                fprintf(output_fp, "%s\n", current_mcro->lines[i]);
            }
        } else {
            /* Write non-macro line as is */
            fprintf(output_fp, "%s", original_line);
        }
    }

    /* Cleanup */
    fclose(output_fp);
    free_mcro_table(table);

    /* If error occurred, delete partial output file */
    if (has_error) {
        remove(output_filename);
        return FALSE;
    }

    return TRUE;
}