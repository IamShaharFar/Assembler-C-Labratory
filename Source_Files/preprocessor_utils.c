/* preprocessor_utils.c */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/globals.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/preprocessor.h"
#include "../Header_Files/preprocessor_utils.h"
#include "../Header_Files/utils.h"

/* Initializes the macro table. */
void init_mcro_table(McroTable *table)
{
    int i;
    table->count = 0;

    /* reset all macro entries */
    for (i = 0; i < MAX_MCROS; i++)
    {
        table->mcros[i].line_count = 0;
    }
}

/* Validates if a given name is a legal macro name. */
ErrorCode is_valid_mcro_name(const char *name)
{
    size_t i;
    char clean_name[MAX_MCRO_NAME_LENGTH];

    strncpy(clean_name, name, MAX_MCRO_NAME_LENGTH - 1); /* copy the name to a new buffer */
    clean_name[MAX_MCRO_NAME_LENGTH - 1] = '\0';
    trim_newline(clean_name); /* remove newline characters */

    /* label must start with an alphabetic character */
    if (name == NULL || !isalpha(name[0]))
    {
        return ERROR_MCRO_ILLEGAL_START;
    }

    /* ensure all characters are alphanumeric or underscore */
    for (i = 1; name[i] != '\0'; i++)
    {
        if (!isalnum(name[i]) && name[i] != '_')
        {
            return ERROR_MCRO_ILLEGAL_CHAR;
        }
    }

    /* check if label is a register name (e.g., r0-r7) */
    if (validate_register_operand(name) == TRUE)
    {
        return ERROR_MCRO_IS_REGISTER;
    }

    /* check if the label is a reserved word */
    for (i = 0; i < RESERVED_WORDS_COUNT; i++)
    {
        if (strcmp(name, reserved_words[i]) == 0)
        {
            return ERROR_MCRO_RESERVED_NAME;
        }
    }

    /* check if the label length is within the allowed limit */
    if (strlen(name) > MAX_MCRO_NAME_LENGTH)
    {
        return ERROR_MCRO_TOO_LONG;
    }

    return ERROR_SUCCESS;
}

/* Adds a new macro to the macro table. */
ErrorCode add_mcro(McroTable *table, const char *name)
{
    int i;
    ErrorCode err;

    err = is_valid_mcro_name(name);

    if (err != ERROR_SUCCESS)
    {
        return err;
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
    strncpy(table->mcros[table->count].name, name, MAX_MCRO_NAME_LENGTH - 1);
    table->mcros[table->count].name[MAX_MCRO_NAME_LENGTH - 1] = '\0';
    table->mcros[table->count].line_count = 0;
    table->count++;

    return ERROR_SUCCESS;
}

/* Adds a line of content to the most recently added macro. */
ErrorCode add_line_to_mcro(McroTable *table, const char *line)
{
    Mcro *current_mcro;

    /* check if there are any macros defined */
    if (table->count == 0)
        return ERROR_MCRO_BEFORE_DEF;

    current_mcro = &table->mcros[table->count - 1]; /* get the last defined macro */

    if (current_mcro->line_count >= MAX_MCRO_LINES)
    {
        return ERROR_MEMORY_ALLOCATION;
    }

    /* add the line to the macro content */
    strncpy(current_mcro->content[current_mcro->line_count], line, MAX_LINE_LENGTH - 1);
    current_mcro->content[current_mcro->line_count][MAX_LINE_LENGTH - 1] = '\0';
    current_mcro->line_count++;

    return ERROR_SUCCESS;
}

/* Processes the content as it would appear in the .am file and writes it to the target file. */
int expand_macros_to_am_file(FILE *source_fp, const char *source_filepath, const McroTable *mcro_table, int *is_valid)
{
    char line[MAX_LINE_LENGTH];
    char temp_line[MAX_LINE_LENGTH];
    char clean_macro_name[MAX_MCRO_NAME_LENGTH];
    char target_filename[MAX_FILENAME_LENGTH];
    char *token, *dot_position; /* dot position for file extension */
    int i, j, is_macro_call, in_macro_def = 0, line_number = 0;
    FILE *target_fp;

    /* create target file name with .am extension */
    strncpy(target_filename, source_filepath, MAX_FILENAME_LENGTH - 1);
    target_filename[MAX_FILENAME_LENGTH - 1] = '\0';
    dot_position = strrchr(target_filename, '.'); /* find the last dot in the filename */

    /* check if the file extension is .as */
    if (!dot_position || strcmp(dot_position, ".as") != 0)
    {
        print_error_no_line(ERROR_MISSING_AS_FILE);
        return FALSE;
    }
    strcpy(dot_position, ".am"); /* replace the extension with .am */

    /* open target file for writing */
    target_fp = fopen(target_filename, "w");
    if (!target_fp)
    {
        print_error_no_line(ERROR_FILE_WRITE);
        return FALSE;
    }

    rewind(source_fp); /* reset the source file pointer to read from the start*/

    /* process the source file line by line */
    while (fgets(line, MAX_LINE_LENGTH, source_fp))
    {
        line_number++;
        strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
        temp_line[MAX_LINE_LENGTH - 1] = '\0';

        token = strtok(temp_line, " \t\n\r"); /* get the first token */
        if (!token || token[0] == ';')
        {
            continue; /* skip empty lines and comment lines */
        }

        /* check if currently inside a macro definition */
        if (in_macro_def)
        {
            if (strcmp(token, "mcroend") == 0)
            {
                in_macro_def = 0; /* end of macro definition */
            }
            continue; /* skip this line */
        }

        if (strcmp(token, "mcro") == 0)
        {
            in_macro_def = 1;
            continue; /* skip this line */
        }

        /* check if the line calls a macro */
        is_macro_call = 0;
        trim_newline(token);

        /* check if the token is a macro name */
        for (i = 0; i < mcro_table->count; i++)
        {
            strncpy(clean_macro_name, mcro_table->mcros[i].name, MAX_MCRO_NAME_LENGTH - 1);
            clean_macro_name[MAX_MCRO_NAME_LENGTH - 1] = '\0';
            trim_newline(clean_macro_name);

            /* check if the token is a macro name */
            if (strcmp(token, clean_macro_name) == 0)
            {
                char *next = advance_past_token(token);
                next = advance_to_next_token(next);

                /* if this is only macro call and not extra text */
                if (*next == '\0' || *next == ';')
                {
                    /* expand macro correctly */
                    for (j = 0; j < mcro_table->mcros[i].line_count; j++)
                    {
                        fprintf(target_fp, "%s\n", mcro_table->mcros[i].content[j]);
                    }
                    is_macro_call = 1;
                    break;
                }
                else
                {
                    print_error(ERROR_MACRO_CALL_EXTRA_TEXT,line_number);
                    *is_valid = FALSE;
                }
            }
        }

        /* only write the line if it's not a macro call */
        if (!is_macro_call)
        {
            char *semicolon_pos = strchr(line, ';');

            /* remove comments */
            if (semicolon_pos)
            {
                *semicolon_pos = '\0';
                strcat(line, "\n"); /* add newline character */
            }
            fprintf(target_fp, "%s", line);
        }
    }

    /* close and flush the target file */
    fflush(target_fp);
    fclose(target_fp);
    return TRUE;
}
