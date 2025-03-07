/* preprocessor_utils.c */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/globals.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/preprocessor.h"
#include "../Header_Files/preprocessor_utils.h"
#include "../Header_Files/utils.h"

/**
 * @brief Initializes the macro table to an empty state.
 *
 * This function initializes the macro table by setting the count to zero and
 * initializing the line count of each macro entry to zero.
 *
 * @param table Pointer to the macro table to initialize.
 */
void init_mcro_table(McroTable *table)
{
    int i;
    table->count = 0;
    for (i = 0; i < MAX_MCROS; i++)
    {
        table->mcros[i].line_count = 0;
    }
}

/**
 * @brief Validates if a given name is a legal macro name.
 *
 * This function checks if the provided name is a valid macro name by comparing it against
 * a list of reserved words. It trims any newline characters from the name before validation.
 *
 * @param name The name to validate.
 * @return TRUE (1) if the name is valid, FALSE (0) otherwise.
 */
int is_valid_mcro_name(const char *name)
{
    size_t i;
    char clean_name[MAX_MCRO_NAME]; 

    strncpy(clean_name, name, MAX_MCRO_NAME - 1); /* Copy the name to a new buffer */
    clean_name[MAX_MCRO_NAME - 1] = '\0';
    trim_newline(clean_name); /* Remove newline characters */

    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(clean_name, reserved_words[i]) == 0)
        {
            return FALSE; /* Name is a reserved word */
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

    if (table->count >= MAX_MCROS)
    {
        return ERROR_MEMORY_ALLOCATION;
    }

    /* Add the macro to the table */
    strncpy(table->mcros[table->count].name, name, MAX_MCRO_NAME - 1);
    table->mcros[table->count].name[MAX_MCRO_NAME - 1] = '\0';
    table->mcros[table->count].line_count = 0;
    table->count++;

    return ERROR_SUCCESS;
}

/**
 * @brief Adds a line of content to the most recently added macro.
 *
 * This function appends a line to the last defined macro in the table. It ensures that the macro
 * table is not empty and that the macro does not exceed the maximum allowed lines.
 *
 * @param table Pointer to the macro table.
 * @param line The line to add to the macro.
 * @return ERROR_SUCCESS if successful, error code otherwise.
 */
ErrorCode add_line_to_mcro(McroTable *table, const char *line)
{
    Mcro *current_mcro;

    /* Check if there are any macros defined */
    if (table->count == 0)
        return ERROR_MCRO_BEFORE_DEF;

    current_mcro = &table->mcros[table->count - 1]; /* Get the last defined macro */

    if (current_mcro->line_count >= MAX_MCRO_LINES)
    {
        return ERROR_MEMORY_ALLOCATION;
    }

    /* Add the line to the macro content */
    strncpy(current_mcro->content[current_mcro->line_count], line, MAX_LINE_LENGTH - 1);
    current_mcro->content[current_mcro->line_count][MAX_LINE_LENGTH - 1] = '\0';
    current_mcro->line_count++;

    return ERROR_SUCCESS;
}

/**
 * @brief Processes the content as it would appear in the .am file and writes it to the target file.
 *
 * This function reads the source file line by line, expands macros when called, and removes macro
 * declarations and calls from the output. It creates a new file with the .am extension and writes
 * the processed content to it.
 *
 * @param source_fp Pointer to the source file.
 * @param source_filepath Path to the source file.
 * @param mcro_table Pointer to the macro table containing defined macros.
 * @return TRUE (1) if processing is successful, FALSE (0) otherwise.
 */
int create_am_file(FILE *source_fp, const char *source_filepath, const McroTable *mcro_table)
{
    char line[MAX_LINE_LENGTH];
    char temp_line[MAX_LINE_LENGTH];
    char clean_macro_name[MAX_MCRO_NAME];
    char target_filename[MAX_FILENAME_LENGTH];
    char *token, *dot_position; /* Dot position for file extension */
    int i, j, is_macro_call, in_macro_def = 0;
    FILE *target_fp;

    /* Create target file name with .am extension */
    strncpy(target_filename, source_filepath, MAX_FILENAME_LENGTH - 1);
    target_filename[MAX_FILENAME_LENGTH - 1] = '\0';
    dot_position = strrchr(target_filename, '.'); /* Find the last dot in the filename */

    /* Check if the file extension is .as */
    if (!dot_position || strcmp(dot_position, ".as") != 0) {
        print_error_no_line(ERROR_MISSING_AS_FILE);
        return FALSE;
    }
    strcpy(dot_position, ".am"); /* Replace the extension with .am */

    /* Open target file for writing */
    target_fp = fopen(target_filename, "w");
    if (!target_fp) {
        print_error_no_line(ERROR_FILE_WRITE);
        return FALSE;
    }

    rewind(source_fp); /* Reset the source file pointer */

    /* Process the source file line by line */
    while (fgets(line, MAX_LINE_LENGTH, source_fp))
    {
        strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
        temp_line[MAX_LINE_LENGTH - 1] = '\0';

        token = strtok(temp_line, " \t\n\r"); /* Get the first token */
        if (!token || token[0] == ';') {
            continue;  /* Skip empty lines and comment lines */
        }

        /* Check if currently inside a macro definition */
        if (in_macro_def) {
            if (strcmp(token, "mcroend") == 0) {
                in_macro_def = 0; /* End of macro definition */
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

        /* Check if the token is a macro name */
        for (i = 0; i < mcro_table->count; i++) {
            strncpy(clean_macro_name, mcro_table->mcros[i].name, MAX_MCRO_NAME - 1);
            clean_macro_name[MAX_MCRO_NAME - 1] = '\0';
            trim_newline(clean_macro_name);

            /* Check if the token is a macro name */
            if (strcmp(token, clean_macro_name) == 0) {
                /* Expand macro correctly */
                for (j = 0; j < mcro_table->mcros[i].line_count; j++) {
                    fprintf(target_fp, "%s\n", mcro_table->mcros[i].content[j]);  
                }
                is_macro_call = 1;
                break;
            }
        }

        /* Only write the line if it's not a macro call */
        if (!is_macro_call) {
            char *semicolon_pos = strchr(line, ';');

            /* Remove comments */
            if (semicolon_pos) {
                *semicolon_pos = '\0';
                strcat(line, "\n"); /* Add newline character */
            }
            fprintf(target_fp, "%s", line);
        }
    }

    /* Close and flush the target file */
    fflush(target_fp);  
    fclose(target_fp);
    return TRUE;
}
