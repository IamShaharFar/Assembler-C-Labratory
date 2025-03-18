/*
 * File: preprocessor.c
 * --------------------
 * Handles preprocessing of assembly files, including macro expansion,
 * path resolution, and file existence checks.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 #include "../Header_Files/preprocessor.h"       /* process_file() */
 #include "../Header_Files/globals.h"            /* MAX_FILENAME_LENGTH, TRUE, FALSE */
 #include "../Header_Files/errors.h"             /* print_error_no_line() */
 #include "../Header_Files/utils.h"              /* Utility functions */
 #include "../Header_Files/preprocessor_utils.h" /* Macro handling functions */
 

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
    size_t dir_len;         /* Length of the directory path */

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
    char *temp_line = NULL, *extra_text;
    size_t buffer_size = MAX_LINE_LENGTH;
    int in_mcro = FALSE, line_number = 0;
    ErrorCode error;
    int ch, pos, is_valid = TRUE;

    /* Allocate memory for the line */
    line = (char *)malloc(buffer_size);
    if (!line)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return FALSE;
    }

    /* Read the file line by line */
    while (!feof(fp))
    {
        pos = 0;

        /* Read characters until end of line or file */
        while ((ch = fgetc(fp)) != EOF && ch != '\n')
        {
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
            line[pos++] = (char)ch;
        }
        line[pos] = '\0';

        /* Check if line is empty */
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

        /* Allocate memory for temp_line */
        free(temp_line); /* Ensure no memory leak before reallocation */
        temp_line = (char *)malloc(strlen(line) + 1);
        if (!temp_line)
        {
            print_error_no_line(ERROR_MEMORY_ALLOCATION);
            free(line);
            return FALSE;
        }

        strcpy(temp_line, line);

        /* Tokenize the line */
        token = strtok(temp_line, " \t\n");
        if (!token)
        {
            continue; /* No need to free here, temp_line will be freed at the next loop iteration */
        }

        token[strcspn(token, "\r\n")] = 0; /* Remove newline characters */

        /* Handle macro end */
        if (strncmp(token, "mcroend", 7) == 0) /* Check first 7 characters */
        {
            /* Find the next non-space character after "mcroend" */
            char *next_char = advance_to_next_token(line) + 7;
            next_char = advance_to_next_token(next_char);
        
            /* Ensure next character is either end of string ('\0') or a comment (';') */
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
        
        /* Handle macro definition */
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
            token[strcspn(token, "\r\n")] = '\0';

            /* Check for unexpected text after macro name */
            extra_text = strtok(NULL, " \t\n");
            if (extra_text && extra_text[0] != ';') /* Only allow comments after */
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

        /* Store macro body */
        if (in_mcro)
        {
            ptr = line;
            while (*ptr == ' ' || *ptr == '\t') /* Skip leading spaces */
                ptr++;

            error = add_line_to_mcro(mcro_table, ptr);
            if (error != ERROR_SUCCESS)
            {
                print_error(error, line_number);
                is_valid = FALSE;
                continue;
            }
        }
    }

    /* Free allocated memory */
    free(line);
    free(temp_line);
    
    /* Create .am file */
    create_am_file(fp, file_path, mcro_table);
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
    char *dir_path;         /* Directory path of the source file */
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
