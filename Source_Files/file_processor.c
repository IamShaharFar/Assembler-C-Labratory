/* file_processor.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Header_Files/globals.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/preprocessor.h"

static int file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

static int count_files_with_basename(const char* filepath) {
    printf("filepath: %s\n", filepath);
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

static int check_as_file_exists(const char* filepath) {
    char filename[MAX_FILENAME_LENGTH];
    sprintf(filename, "%s.as", filepath);
    return file_exists(filename);
}

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

static char* get_directory_path(const char* filepath) {
    char* dir_path;
    const char* last_slash;
    size_t dir_len;
    
    /* Find last occurrence of '/' or '\\' */
    last_slash = strrchr(filepath, '/');
    if (!last_slash) {
        last_slash = strrchr(filepath, '\\');
    }
    
    if (!last_slash) {
        /* No directory part, return current directory */
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