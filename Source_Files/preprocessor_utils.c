/* preprocessor_utils.c */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/globals.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/preprocessor.h"
#include "../Header_Files/preprocessor_utils.h"
#include "../Header_Files/utils.h"

/* Reserved words in assembly language */
const char *reserved_words[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
    "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    ".data", ".string", ".entry", ".extern"};

#define RESERVED_WORDS_COUNT (sizeof(reserved_words) / sizeof(reserved_words[0]))

/**
 * @brief Initializes the macro table to an empty state.
 *
 * This function sets the count of macros to zero and clears the line count for each macro.
 *
 * @param table Pointer to the macro table to initialize.
 */
void init_mcro_table(McroTable *table)
{
    int i;
    table->count = 0;
    for (i = 0; i < MAX_MACRO_COUNT; i++)
    {
        table->mcros[i].line_count = 0;
    }
}

/**
 * @brief Validates if a given name is a legal macro name.
 *
 * The function checks if the provided name conflicts with reserved words.
 *
 * @param name The name to validate.
 * @return TRUE if the name is valid, FALSE otherwise.
 */
int is_valid_mcro_name(const char *name)
{
    size_t i;
    char clean_name[MAX_MACRO_NAME];

    strncpy(clean_name, name, MAX_MACRO_NAME - 1);
    clean_name[MAX_MACRO_NAME - 1] = '\0';
    trim_newline(clean_name);

    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(clean_name, reserved_words[i]) == 0)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * @brief Adds a new macro to the macro table.
 *
 * If the macro name is valid and not a duplicate, it will be added to the macro table.
 *
 * @param table Pointer to the macro table.
 * @param name The name of the macro to add.
 * @return ERROR_SUCCESS if successful, error code otherwise.
 */
ErrorCode add_mcro(McroTable *table, const char *name)
{
    int i;

    if (!is_valid_mcro_name(name))
    {
        return ERROR_MCRO_ILLEGAL_NAME;
    }

    for (i = 0; i < table->count; i++)
    {
        if (strcmp(table->mcros[i].name, name) == 0)
        {
            return ERROR_MCRO_DUPLICATE;
        }
    }

    if (table->count >= MAX_MACRO_COUNT)
    {
        return ERROR_MEMORY_ALLOCATION;
    }

    strncpy(table->mcros[table->count].name, name, MAX_MACRO_NAME - 1);
    table->mcros[table->count].name[MAX_MACRO_NAME - 1] = '\0';
    table->mcros[table->count].line_count = 0;
    table->count++;
    return ERROR_SUCCESS;
}

/**
 * @brief Adds a line of content to the most recently added macro.
 *
 * This function appends a line to the last defined macro in the table.
 *
 * @param table Pointer to the macro table.
 * @param line The line to add to the macro.
 * @return ERROR_SUCCESS if successful, error code otherwise.
 */
ErrorCode add_line_to_mcro(McroTable *table, const char *line)
{
    Mcro *current_mcro;

    if (table->count == 0)
        return ERROR_MCRO_BEFORE_DEF;

    current_mcro = &table->mcros[table->count - 1];
    if (current_mcro->line_count >= MAX_MACRO_LINES)
    {
        return ERROR_MEMORY_ALLOCATION;
    }

    strncpy(current_mcro->content[current_mcro->line_count], line, MAX_LINE_LENGTH - 1);
    current_mcro->content[current_mcro->line_count][MAX_LINE_LENGTH - 1] = '\0';
    current_mcro->line_count++;
    return ERROR_SUCCESS;
}

/**
 * @brief Prints all macros stored in the macro table.
 *
 * This function iterates through the macro table and prints the name and content of each macro.
 *
 * @param table Pointer to the macro table to print.
 */
void print_mcro_table(const McroTable *table)
{
    int i, j;
    printf("\nDeclared macros:\n");
    for (i = 0; i < table->count; i++)
    {
        printf("Macro: %s\n", table->mcros[i].name);
        printf("Line count: %d\n", table->mcros[i].line_count);
        printf("----------\n");
        for (j = 0; j < table->mcros[i].line_count; j++)
        {
            printf("%s", table->mcros[i].content[j]);
        }
        printf("\n");
    }
}

/**
 * @brief Processes the content as it would appear in the .am file and saves it.
 *
 * This function simulates the .am file content by processing the source file. It expands macros
 * when called and removes macro declarations, macro calls, empty lines, and comment lines from the output.
 * The processed content is saved as a .am file with the same name as the source file.
 *
 * @param source_fp Pointer to the source file.
 * @param source_filepath The full path of the source file to create the corresponding .am file.
 * @param mcro_table Pointer to the macro table containing defined macros.
 * @return TRUE if processing is successful, FALSE otherwise.
 */
int create_am_file(FILE *source_fp, const char *source_filepath, const McroTable *mcro_table)
{
    char line[MAX_LINE_LENGTH];
    char temp_line[MAX_LINE_LENGTH];
    char clean_macro_name[MAX_MCRO_NAME];
    char target_filename[MAX_FILENAME_LENGTH];
    char *token, *dot_position;
    int i, j, is_macro_call, in_macro_def = 0;
    FILE *target_fp;

    /* Create target file name with .am extension in the same directory */
    strncpy(target_filename, source_filepath, MAX_FILENAME_LENGTH - 1);
    target_filename[MAX_FILENAME_LENGTH - 1] = '\0';
    dot_position = strrchr(target_filename, '.');
    if (!dot_position || strcmp(dot_position, ".as") != 0) {
        print_error_no_line(ERROR_MISSING_AS_FILE);
        return FALSE;
    }
    strcpy(dot_position, ".am");

    /* Open target file for writing */
    target_fp = fopen(target_filename, "w");
    if (!target_fp) {
        print_error_no_line(ERROR_FILE_WRITE);
        return FALSE;
    }

    rewind(source_fp);

    while (fgets(line, MAX_LINE_LENGTH, source_fp))
    {
        strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
        temp_line[MAX_LINE_LENGTH - 1] = '\0';

        token = strtok(temp_line, " \t\n\r");
        if (!token || token[0] == ';') {
            continue;  /* Skip empty lines and comment lines */
        }

        /* Check if currently inside a macro definition to skip lines */
        if (in_macro_def) {
            if (strcmp(token, "mcroend") == 0) {
                in_macro_def = 0;
            }
            continue;
        }

        /* Check if the line is a macro declaration */
        if (strcmp(token, "mcro") == 0) {
            in_macro_def = 1;
            continue;
        }

        /* Check if the line calls a macro */
        is_macro_call = 0;
        trim_newline(token);

        for (i = 0; i < mcro_table->count; i++) {
            strncpy(clean_macro_name, mcro_table->mcros[i].name, MAX_MCRO_NAME - 1);
            clean_macro_name[MAX_MCRO_NAME - 1] = '\0';
            trim_newline(clean_macro_name);

            if (strcmp(token, clean_macro_name) == 0) {
                for (j = 0; j < mcro_table->mcros[i].line_count; j++) {
                    fprintf(target_fp, "%s", mcro_table->mcros[i].content[j]);
                }
                is_macro_call = 1;
                break;
            }
        }

        /* Only write the line if it's not a macro call */
        if (!is_macro_call) {
            fprintf(target_fp, "%s", line);
        }
    }

    fclose(target_fp);
    return TRUE;
}
