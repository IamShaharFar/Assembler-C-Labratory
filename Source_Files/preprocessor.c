/* preprocessor.c */
#include <stdio.h>   
#include <unistd.h>    
#include <string.h>
#include <ctype.h>
#include "../Header_Files/preprocessor.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/preprocessor_utils.h"

static McroTable mcro_table;

void print_mcro_if_called(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    char temp_line[MAX_LINE_LENGTH];
    char *token;
    int i, j;

    rewind(fp);

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        strncpy(temp_line, line, MAX_LINE_LENGTH - 1);
        temp_line[MAX_LINE_LENGTH - 1] = '\0';
        
        token = strtok(temp_line, " \t\n\r");
        if (!token)
            continue;

        trim_newline(token);

        for (i = 0; i < mcro_table.count; i++)
        {
            char clean_macro_name[MAX_MACRO_NAME];
            strncpy(clean_macro_name, mcro_table.mcros[i].name, MAX_MACRO_NAME - 1);
            clean_macro_name[MAX_MACRO_NAME - 1] = '\0';
            trim_newline(clean_macro_name);

            if (strcmp(token, clean_macro_name) == 0)
            {
                printf("Expanding macro: %s\n", clean_macro_name);
                for (j = 0; j < mcro_table.mcros[i].line_count; j++)
                {
                    printf("%s", mcro_table.mcros[i].content[j]);
                }
                printf("\n");
            }
        }
    }
}

/**
 * @brief Processes an assembly file to identify and store macro definitions
 * @param fp File pointer to the assembly source file
 */
void process_as_file(FILE *fp, const char *file_path)
{
    char line[MAX_LINE_LENGTH];
    char temp_line[MAX_LINE_LENGTH];
    char *token;
    int in_mcro = FALSE, line_number = 0;
    ErrorCode error;

    init_mcro_table(&mcro_table);

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        line_number++;

        if (check_line_length_exceeded(line))
        {
            print_error(ERROR_LINE_TOO_LONG, line_number);
            continue;
        }

        strcpy(temp_line, line);
        token = strtok(temp_line, " \t\n");
        if (!token)
            continue;

        token[strcspn(token, "\r\n")] = 0;

        if (strcmp(token, "mcroend") == 0)
        {
            in_mcro = FALSE;
            continue;
        }

        if (strcmp(token, "mcro") == 0)
        {
            token = strtok(NULL, " \t\n");
            if (!token)
            {
                print_error(ERROR_MCRO_NO_NAME, line_number);
                continue;
            }

            error = add_mcro(&mcro_table, token);
            if (error != ERROR_SUCCESS)
            {
                print_error(error, line_number);
                return;
            }

            in_mcro = TRUE;
            continue;
        }

        if (in_mcro)
        {
            char *trimmed_line = line;
            while (*trimmed_line == ' ' || *trimmed_line == '\t')
                trimmed_line++;

            error = add_line_to_mcro(&mcro_table, trimmed_line);
            if (error != ERROR_SUCCESS)
            {
                print_error(error, line_number);
                return;
            }
        }
    }

    /* Save processed content as .am file */
    create_am_file(fp, file_path, &mcro_table);
}
