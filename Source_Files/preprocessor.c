/* preprocessor.c */
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
 * @brief Checks if a given file exists.
 *
 * This function attempts to open the specified file in read mode to check if it exists.
 *
 * @param filename The name of the file to check.
 * @return TRUE (1) if the file exists, FALSE (0) otherwise.
 */
static int file_exists(const char *filename)
{
    FILE *file = fopen(filename, "r"); /* Try to open the file in read mode */
    if (file) /* File exists */
    {
        fclose(file); /* Close the file */
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Checks if a .as file exists for the given filepath.
 *
 * This function constructs the filename by appending ".as" to the given filepath
 * and checks if the resulting file exists.
 *
 * @param filepath Base filepath.
 * @return TRUE (1) if the .as file exists, FALSE (0) otherwise.
 */
static int check_as_file_exists(const char *filepath)
{
    char filename[MAX_FILENAME_LENGTH];
    sprintf(filename, "%s.as", filepath); /* Append .as extension to the filename */
    return file_exists(filename); /* Check if the file exists */
}

/**
 * @brief Constructs the full path of the source .as file.
 *
 * This function constructs the full path of the source .as file by appending ".as" to the given filepath
 * if it does not already have the ".as" extension. The caller is responsible for freeing the allocated memory.
 *
 * @param filepath Base filepath.
 * @return Pointer to the full source path string. Returns NULL if memory allocation fails.
 */
static char *get_full_source_path(const char *filepath)
{
    char *full_path; 
    size_t len = strlen(filepath); /* Get the length of the filepath */

    full_path = (char *)malloc(MAX_FILENAME_LENGTH); /* Allocate memory for the full path */
    if (!full_path)
    {
        /* Memory allocation failed */
        return NULL;
    }

    /* Check if the filepath already has the .as extension */
    if (len > 3 && strcmp(filepath + len - 3, ".as") == 0)
    {
        /* Copy the filepath to the full path */
        strncpy(full_path, filepath, MAX_FILENAME_LENGTH - 1);
    }
    else
    {
        /* Append .as extension to the filepath */
        sprintf(full_path, "%s.as", filepath);
    }

    return full_path;
}

/**
 * @brief Extracts the directory path from the given filepath.
 *
 * This function extracts the directory path from the given filepath by finding the last occurrence
 * of the '/' character. If no '/' is found, it returns the current directory (".").
 * The caller is responsible for freeing the allocated memory.
 *
 * @param filepath Full filepath.
 * @return Pointer to the directory path string. Returns NULL if memory allocation fails.
 */
static char *get_directory_path(const char *filepath)
{
    char *dir_path;
    const char *last_slash; /* Pointer to the last occurrence of '/' in the filepath */
    size_t dir_len; /* Length of the directory path */

    /* Find last occurrence of '/' */
    last_slash = strrchr(filepath, '/');

    /* Check if no '/' is found */
    if (!last_slash)
    {
        /* Return the current directory */
        dir_path = (char *)malloc(2);
        if (dir_path)
        {
            strcpy(dir_path, ".");
        }
        return dir_path;
    }

    dir_len = last_slash - filepath; /* Calculate the length of the directory path */
    dir_path = (char *)malloc(dir_len + 1); 

    /* Check if memory allocation failed */
    if (!dir_path)
    {
        return NULL;
    }

    strncpy(dir_path, filepath, dir_len); /* Copy the directory path */
    dir_path[dir_len] = '\0';

    return dir_path;
}

/**
 * @brief Processes the macros in the given assembly file.
 *
 * This function reads the assembly file line by line, processes macro definitions and their content,
 * and stores them in the provided macro table. It handles memory allocation for reading lines and
 * ensures that lines do not exceed the maximum allowed length. The function also creates an output
 * file with the processed content.
 *
 * @param fp File pointer to the assembly source file.
 * @param file_path Path to the source file.
 * @param mcro_table Pointer to the macro table.
 * @return TRUE (1) if the file is valid and processed successfully, FALSE (0) otherwise.
 */
int process_as_file(FILE *fp, const char *file_path, McroTable *mcro_table)
{
    char *line = NULL, *token, *ptr;
    char*temp_line; /* Temporary line to store the line content */
    size_t buffer_size = MAX_LINE_LENGTH; 
    int in_mcro = FALSE, line_number = 0;
    ErrorCode error;
    int ch, pos, is_valid = TRUE;

    init_mcro_table(mcro_table); /* Initialize the macro table */

    line = (char *)malloc(buffer_size); /* Allocate memory for the line */

    if (!line)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        is_valid = FALSE;
        return is_valid;
    }

    /* Read the file line by line */
    while (!feof(fp))
    {
        pos = 0;

        /* Read until the end of the line or the end of the file */
        while ((ch = fgetc(fp)) != EOF && ch != '\n')
        {
            if (pos >= buffer_size - 1) /* Check if the line exceeds the buffer size */
            {
                buffer_size *= 2;
                line = (char *)realloc(line, buffer_size); 
                if (!line)
                {
                    print_error_no_line(ERROR_MEMORY_ALLOCATION);
                    is_valid = FALSE;
                    return is_valid;
                }
            }
            line[pos++] = (char)ch; /* Store the character in the line */
        }
        line[pos] = '\0';

        /* Check if the line is empty */
        if (pos == 0 && ch == EOF) 
            break;
        
        line_number++;
        
        /* Check if the line exceeds the maximum allowed length */
        if (pos >= MAX_LINE_LENGTH)
        {
            print_error(ERROR_LINE_TOO_LONG, line_number);
            is_valid = FALSE;
            continue;
        }

        temp_line = (char *)malloc(strlen(line) + 1); /* Allocate memory for the temporary line */
        
        if (!temp_line)
        {
            print_error_no_line(ERROR_MEMORY_ALLOCATION);
            free(line);
            is_valid = FALSE;
            return is_valid;
        }

        strcpy(temp_line, line); /* Copy the line to the temporary line */

        if (!temp_line)
        {
            print_error_no_line(ERROR_MEMORY_ALLOCATION);
            free(line);
            is_valid = FALSE;
            return is_valid;
        }


        /* Tokenize the line to get the first token, using " \t\n" as delimiters 
        ** to handle spaces, tabs, and newlines */
        token = strtok(temp_line, " \t\n"); 
        if (!token)
        {
            free(temp_line);
            continue;
        }

        token[strcspn(token, "\r\n")] = 0; /* Remove newline characters from the token */

        if (strcmp(token, "mcroend") == 0)
        {
            in_mcro = FALSE; 
            free(temp_line);
            continue;
        }

        if (strcmp(token, "mcro") == 0)
        {
            /* Retrieve the next token from the string, using space, tab,
            ** or newline as delimiters, to get the macro name */
            token = strtok(NULL, " \t\n"); 
            if (!token)
            {
                print_error(ERROR_MCRO_NO_NAME, line_number);
                free(temp_line);
                continue;
            }
            token[strcspn(token, "\r\n")] = '\0'; /* Remove newline characters from the token */

            error = add_mcro(mcro_table, token);

            /* Check if the macro name is invalid or a duplicate */
            if (error == ERROR_MCRO_ILLEGAL_NAME || error == ERROR_MCRO_DUPLICATE)
            {
                print_error(error, line_number);
                free(temp_line);
                is_valid = FALSE;
            }

            /* Check if memory allocation failed */
            else if (error != ERROR_SUCCESS)
            {
                print_error(error, line_number);
                free(temp_line);
                is_valid = FALSE;
                return is_valid;
            }

            in_mcro = TRUE;
            free(temp_line);
            continue;
        }

        /* Check if currently inside a macro definition */
        if (in_mcro)
        {
            ptr = line;

            /* Skip leading spaces and tabs */
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;

            error = add_line_to_mcro(mcro_table, ptr);

            /* Check if memory allocation failed */
            if (error != ERROR_SUCCESS)
            {
                print_error(error, line_number);
                free(temp_line);
                is_valid = FALSE;
                return is_valid;
            }
        }

        free(temp_line); /* Free the temporary line */
    }

    free(line);
    create_am_file(fp, file_path, mcro_table); /* Create the .am file */
    return is_valid;
}

/**
 * @brief Processes the given assembly file.
 *
 * This function checks the validity of the given filepath, constructs the full source path,
 * and processes the assembly file by reading its content and handling macro definitions.
 * It also handles memory allocation for paths and ensures proper cleanup.
 *
 * @param filepath Path to the file.
 * @param mcro_table Pointer to the macro table.
 * @return TRUE (1) if processing is successful, FALSE (0) otherwise.
 */
int process_file(const char *filepath, McroTable *mcro_table)
{
    char *full_source_path; /* Full path of the source file */
    char *dir_path; /* Directory path of the source file */
    FILE *fp;
    int result; 

    /* Check if the filename is too long */
    if (strlen(filepath) > MAX_FILENAME_LENGTH - 4)
    {
        print_error(ERROR_FILENAME_TOO_LONG, 0);
        return FALSE;
    }

    /* Check if the .as file exists */
    if (!check_as_file_exists(filepath))
    {
        print_error(ERROR_MISSING_AS_FILE, 0);
        return FALSE;
    }

    full_source_path = get_full_source_path(filepath);

    /* Check if memory allocation failed */
    if (!full_source_path)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return FALSE;
    }

    dir_path = get_directory_path(filepath);

    /* Check if memory allocation failed */
    if (!dir_path)
    {
        free(full_source_path);
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return FALSE;
    }

    /* Open the source file for reading */
    fp = fopen(full_source_path, "r");

    /* Check if the file could not be opened */
    if (!fp)
    {
        free(full_source_path);
        free(dir_path);
        print_error_no_line(ERROR_FILE_READ);
        return FALSE;
    }

    result = process_as_file(fp, full_source_path, mcro_table); /* Process the assembly file and return the result if is a valid file */

    /* Cleanup */
    fclose(fp);
    free(full_source_path);
    free(dir_path);

    return result;
}
