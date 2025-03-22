/* Source_Files/preprocessor.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/preprocessor.h"       
#include "../Header_Files/globals.h"            
#include "../Header_Files/errors.h"           
#include "../Header_Files/utils.h"            
#include "../Header_Files/preprocessor_utils.h" 

/**
 * @brief Checks if a given file exists by attempting to open it.
 *
 * @param filename The name of the file to check.
 * @return TRUE (1) if the file exists, FALSE (0) otherwise.
 */
static int file_exists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Checks if an assembly (.as) file exists for the given filepath.
 *
 * @param filepath Base filepath.
 * @return TRUE (1) if the file exists, FALSE (0) otherwise.
 */
static int check_as_file_exists(const char *filepath)
{
    char filename[MAX_FILENAME_LENGTH];
    sprintf(filename, "%s.as", filepath); /* Append .as extension */
    return file_exists(filename);
}

/**
 * @brief Constructs the full path of the source .as file.
 *
 * Ensures the given filepath has a .as extension, appending it if necessary.
 * Caller must free allocated memory.
 *
 * @param filepath Base filepath.
 * @return Pointer to allocated string containing full path or NULL on failure.
 */
static char *get_full_source_path(const char *filepath)
{
    char *full_path = (char *)malloc(MAX_FILENAME_LENGTH + 9); /* +9 for "./Inputs/" */
    size_t len = strlen(filepath);

    if (!full_path)
    {
        return NULL;
    }

    /* check if the filepath already has the .as extension */
    if (len > 3 && strcmp(filepath + len - 3, ".as") == 0)
    {
        strncpy(full_path, filepath, MAX_FILENAME_LENGTH - 1);
    }
    else
    {
        sprintf(full_path, "%s.as", filepath);
    }
    return full_path;
}

/**
 * @brief Extracts directory path from a given filepath.
 *
 * Returns "./" if no directory is present in the path.
 * Caller must free allocated memory.
 *
 * @param filepath Full filepath.
 * @return Pointer to allocated string containing directory path, or NULL on failure.
 */
static char *get_directory_path(const char *filepath)
{
    char *dir_path;
    const char *last_slash = strrchr(filepath, '/'); /* locate the last '/' in filepath */
    size_t dir_len;                                  /* length of the directory path */

    if (!last_slash) /* no directory separator found, return "./" */
    {
        dir_path = (char *)malloc(2);
        if (dir_path)
        {
            strcpy(dir_path, ".");
        }
        return dir_path;
    }

    dir_len = last_slash - filepath; /* compute directory length */
    dir_path = (char *)malloc(dir_len + 1);

    if (!dir_path) /* memory allocation failure */
    {
        return NULL;
    }

    strncpy(dir_path, filepath, dir_len);
    dir_path[dir_len] = '\0'; /* null-terminate the string */

    return dir_path;
}

/* Processes macros in an assembly file. */
int process_as_file(FILE *fp, const char *file_path, McroTable *mcro_table)
{
    char *line = NULL, *token, *ptr;
    char *temp_line = NULL, *extra_text;
    size_t buffer_size = MAX_LINE_LENGTH;
    int in_mcro = FALSE, line_number = 0;
    ErrorCode error;
    int ch, pos, is_valid = TRUE;

    /* allocate memory for the line */
    line = (char *)malloc(buffer_size);
    if (!line)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return FALSE;
    }

    /* read file line by line */
    while (!feof(fp))
    {
        pos = 0;

        /* read until newline or end of file */
        while ((ch = fgetc(fp)) != EOF && ch != '\n')
        {
            /* realloc if line is too long */
            if (pos >= buffer_size - 1)
            {
                char *new_line;
                buffer_size *= 2;
                new_line = (char *)realloc(line, buffer_size);
                if (!new_line)
                {
                    print_error_no_line(ERROR_MEMORY_ALLOCATION);
                    free(line);
                    return FALSE;
                }
                line = new_line;
            }
            line[pos++] = (char)ch; /* put the char and skip pos to the next char*/
        }
        line[pos] = '\0'; /* ensure null termination */

        /* check if empty line */
        if (pos == 0 && ch == EOF)
            break;

        line_number++;

        /* find last non-whitespace character */
        
        /* check if line exceeds max length after trimming trailing spaces */
        if (pos >= MAX_LINE_LENGTH)
        {
            print_error(ERROR_LINE_TOO_LONG, line_number);
            is_valid = FALSE;
            continue;
        }

        /* free previous temp_line before reuse */
        free(temp_line);
        temp_line = (char *)malloc(strlen(line) + 1);
        if (!temp_line)
        {
            print_error_no_line(ERROR_MEMORY_ALLOCATION);
            free(line);
            return FALSE;
        }

        strcpy(temp_line, line);

        token = strtok(temp_line, " \t\n");
        if (!token)
        {
            continue;
        }

        trim_newline(token);

        /* handle macro end */
        if (strncmp(token, "mcroend", 7) == 0)
        {
            /* look ahead for extra junk text */
            char *next_char = advance_to_next_token(line) + 7;
            next_char = advance_to_next_token(next_char);

            /* ensure next character is either end of string ('\0') or a comment (';') */
            if (*next_char == '\0' || *next_char == ';')
            {
                in_mcro = FALSE;
                continue;
            }
            else
            {
                print_error(ERROR_EXTRA_TEXT_AFTER_COMMAND, line_number);
                is_valid = FALSE;
                continue;
            }
        }

        /* handle macro definition */
        if (strcmp(token, "mcro") == 0)
        {
            in_mcro = TRUE;
            token = strtok(NULL, " \t\n");
            if (!token)
            {
                print_error(ERROR_MCRO_NO_NAME, line_number);
                is_valid = FALSE;
                continue;
            }
            trim_newline(token);

            /* check for unexpected text after macro name */
            extra_text = strtok(NULL, " \t\n");
            if (extra_text && extra_text[0] != ';') /* only allow comments after */
            {
                print_error(ERROR_MCRO_UNEXPECTED_TEXT, line_number);
                is_valid = FALSE;
                continue;
            }

            error = add_mcro(mcro_table, token);
            if (error != ERROR_SUCCESS && error != ERROR_MEMORY_ALLOCATION)
            {
                print_error(error, line_number);
                is_valid = FALSE;
                continue;
            }

            if (error == ERROR_MEMORY_ALLOCATION)
            {
                print_error(error, line_number);
                free(line);
                free(temp_line);
                return FALSE;
            }

            continue;
        }

        /* store macro body */
        if (in_mcro)
        {
            ptr = line;
            ptr = advance_to_next_token(ptr);

            error = add_line_to_mcro(mcro_table, ptr);
            if (error != ERROR_SUCCESS)
            {
                print_error(error, line_number);
                is_valid = FALSE;
                continue;
            }
        }
    }

    /* cleanup */
    free(line);
    free(temp_line);

    /* Create .am file */
    expand_macros_to_am_file (fp, file_path, mcro_table, &is_valid);
    return is_valid;
}

/* Prepocesses an assembly file. */
int process_file(const char *filepath, McroTable *mcro_table)
{
    char *full_source_path;
    char *dir_path;
    FILE *fp;
    int result;

    /* check filename length after ".as/0"*/
    if (strlen(filepath) > MAX_FILENAME_LENGTH - 4)
    {
        print_error(ERROR_FILENAME_TOO_LONG, 0);
        return FALSE;
    }

    /* verify file exists */
    if (!check_as_file_exists(filepath))
    {
        print_error(ERROR_FILE_NOT_EXIST, 0);
        return FALSE;
    }

    full_source_path = get_full_source_path(filepath);

    /* check if memory allocation failed */
    if (!full_source_path)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return FALSE;
    }

    dir_path = get_directory_path(filepath);

    /* check if memory allocation failed */
    if (!dir_path)
    {
        free(full_source_path);
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return FALSE;
    }

    /* open the source file for reading */
    fp = fopen(full_source_path, "r");

    /* check if the file could not be opened */
    if (!fp)
    {
        free(full_source_path);
        free(dir_path);
        print_error_no_line(ERROR_FILE_READ);
        return FALSE;
    }

    /* process macros */
    result = process_as_file(fp, full_source_path, mcro_table);

    /* cleanup */
    fclose(fp);
    free(full_source_path);
    free(dir_path);

    return result;
}
