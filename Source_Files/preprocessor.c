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

static McroTable mcro_table;

/**
 * @brief Checks if a given file exists.
 *
 * @param filename Name of the file to check.
 * @return 1 if the file exists, 0 otherwise.
 */
static int file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

/**
 * @brief Counts the number of files with specific extensions (.as, .am, .ob).
 *
 * @param filepath Base filepath to check.
 * @return Number of files found with matching extensions.
 */
static int count_files_with_basename(const char* filepath) {
    char test_filename[MAX_FILENAME_LENGTH];
    const char* extensions[] = {".as", ".am", ".ob"};
    int count = 0;
    size_t i;

    for (i = 0; i < sizeof(extensions)/sizeof(extensions[0]); i++) {
        sprintf(test_filename, "%s%s", filepath, extensions[i]);
        if (file_exists(test_filename)) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Checks if a .as file exists for the given filepath.
 *
 * @param filepath Base filepath.
 * @return 1 if the .as file exists, 0 otherwise.
 */
static int check_as_file_exists(const char* filepath) {
    char filename[MAX_FILENAME_LENGTH];
    sprintf(filename, "%s.as", filepath);
    return file_exists(filename);
}

/**
 * @brief Constructs the full path of the source .as file.
 *
 * @param filepath Base filepath.
 * @return Pointer to the full source path string. Caller must free the memory.
 */
static char* get_full_source_path(const char* filepath) {
    char* full_path;
    size_t len = strlen(filepath);
    
    full_path = (char*)malloc(MAX_FILENAME_LENGTH);
    if (!full_path) {
        return NULL;
    }

    if (len > 3 && strcmp(filepath + len - 3, ".as") == 0) {
        strncpy(full_path, filepath, MAX_FILENAME_LENGTH - 1);
    } else {
        sprintf(full_path, "%s.as", filepath);
    }
    
    return full_path;
}

/**
 * @brief Extracts the directory path from the given filepath.
 *
 * @param filepath Full filepath.
 * @return Pointer to the directory path string. Caller must free the memory.
 */
static char* get_directory_path(const char* filepath) {
    char* dir_path;
    const char* last_slash;
    size_t dir_len;
    
    /* Find last occurrence of '/' */
    last_slash = strrchr(filepath, '/');
    if (!last_slash) {
        dir_path = (char*)malloc(2);
        if (dir_path) {
            strcpy(dir_path, ".");
        }
        return dir_path;
    }
    
    dir_len = last_slash - filepath;
    dir_path = (char*)malloc(dir_len + 1);
    if (!dir_path) {
        return NULL;
    }
    
    strncpy(dir_path, filepath, dir_len);
    dir_path[dir_len] = '\0';
    
    return dir_path;
}

/**
 * @brief Processes the macros in the given assembly file.
 *
 * @param fp File pointer to the assembly source file.
 * @param file_path Path to the source file.
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

/**
 * @brief Processes the given assembly file.
 *
 * @param filepath Path to the file.
 * @return 1 if processing is successful, 0 otherwise.
 */
int process_file(const char* filepath) {
    char* full_source_path;
    char* dir_path;
    FILE* fp;

    if (strlen(filepath) > MAX_FILENAME_LENGTH - 4) {
        print_error(ERROR_FILENAME_TOO_LONG, 0);
        return 0;
    }

    if (count_files_with_basename(filepath) == 0) {
        print_error(ERROR_FILE_NOT_EXIST, 0);
        return 0;
    }

    if (!check_as_file_exists(filepath)) {
        print_error(ERROR_MISSING_AS_FILE, 0);
        return 0;
    }

    full_source_path = get_full_source_path(filepath);
    if (!full_source_path) {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return 0;
    }

    dir_path = get_directory_path(filepath);
    if (!dir_path) {
        free(full_source_path);
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return 0;
    }

    /* Open source file for preprocessing */
    fp = fopen(full_source_path, "r");
    if (!fp) {
        free(full_source_path);
        free(dir_path);
        print_error(ERROR_FILE_READ, 0);
        return 0;
    }

    /* Process macros */
    process_as_file(fp, full_source_path);

    /* Cleanup */
    fclose(fp);
    free(full_source_path);
    free(dir_path);

    return 1;
}