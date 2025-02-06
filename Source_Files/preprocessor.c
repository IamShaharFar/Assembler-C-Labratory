/* preprocessor.c */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Header_Files/preprocessor.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/utils.h"

static McroTable mcro_table;

static const char *reserved_words[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
    "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    ".data", ".string", ".entry", ".extern"
};

#define RESERVED_WORDS_COUNT (sizeof(reserved_words) / sizeof(reserved_words[0]))

/**
 * @brief Initializes a macro table to empty state
 * @param table Pointer to the macro table to initialize
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

void trim_newline(char *str) {
    char *end;
    
    /* Return if str is NULL or empty */
    if (str == NULL || *str == '\0') return;
    
    /* Trim trailing whitespace and newlines */
    end = str + strlen(str) - 1;
    while (end > str && (isspace((unsigned char)*end) || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
}

int is_valid_mcro_name(const char *name) {
    size_t i;
    char clean_name[MAX_MCRO_NAME];
    strncpy(clean_name, name, MAX_MCRO_NAME - 1);
    clean_name[MAX_MCRO_NAME - 1] = '\0';
    trim_newline(clean_name); 

    for (i = 0; i < RESERVED_WORDS_COUNT; i++) {
        int cmp_result = strcmp(clean_name, reserved_words[i]);
        if (cmp_result == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * @brief Adds a new macro to the table if it doesn't already exist
 * @param table Pointer to the macro table
 * @param name The name of the macro to add
 * @return ERROR_SUCCESS if successful, ERROR_MCRO_DUPLICATE if macro already exists,
 *         ERROR_MEMORY_ALLOCATION if table is full
 */
ErrorCode add_mcro(McroTable *table, const char *name)
{
    int i;

    if (is_valid_mcro_name(name) == FALSE)
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

    strncpy(table->mcros[table->count].name, name, MAX_MCRO_NAME - 1);
    table->mcros[table->count].name[MAX_MCRO_NAME - 1] = '\0';
    table->mcros[table->count].line_count = 0;
    table->count++;
    return ERROR_SUCCESS;
}

ErrorCode add_line_to_mcro(McroTable *table, const char *line)
{
    Mcro *current_mcro;

    if (table->count == 0)
        return ERROR_MCRO_BEFORE_DEF;

    current_mcro = &table->mcros[table->count - 1];
    if (current_mcro->line_count >= MAX_MCRO_LINES)
    {
        return ERROR_MEMORY_ALLOCATION;
    }

    strncpy(current_mcro->content[current_mcro->line_count], line, MAX_LINE_LENGTH - 1);
    current_mcro->content[current_mcro->line_count][MAX_LINE_LENGTH - 1] = '\0';
    current_mcro->line_count++;
    return ERROR_SUCCESS;
}

/**
 * @brief Prints all macro names stored in the table
 * @param table Pointer to the macro table to print
 */
void print_mcro_table(const McroTable *table)
{
    int i, j;
    printf("\nDeclared macros:\n");
    for (i = 0; i < table->count; i++)
    {
        printf("Mcro: %s\n", table->mcros[i].name);
        printf("Line count: %d\n", table->mcros[i].line_count);
        printf("----------\n");
        for (j = 0; j < table->mcros[i].line_count; j++)
        {
            printf("%s", table->mcros[i].content[j]);
        }
        printf("\n");
    }
}

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
        
        /* Use \n\r for tokenization to handle different line endings */
        token = strtok(temp_line, " \t\n\r");
        if (!token)
            continue;

        /* Clean the token */
        trim_newline(token);

        /* Clean both strings before comparison */
        for (i = 0; i < mcro_table.count; i++)
        {
            char clean_macro_name[MAX_MCRO_NAME];
            strncpy(clean_macro_name, mcro_table.mcros[i].name, MAX_MCRO_NAME - 1);
            clean_macro_name[MAX_MCRO_NAME - 1] = '\0';
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

int create_am_file(FILE *source_fp, const char *source_filename) {
    FILE *target_fp;
    char line[MAX_LINE_LENGTH];
    char target_filename[MAX_FILENAME_LENGTH];
    char *dot_position;
    
    /* Create target filename by replacing .as with .am */
    strncpy(target_filename, source_filename, MAX_FILENAME_LENGTH - 1);
    target_filename[MAX_FILENAME_LENGTH - 1] = '\0';
    
    dot_position = strrchr(target_filename, '.');
    if (!dot_position) {
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
    
    /* Reset source file pointer to beginning */
    rewind(source_fp);
    
    /* Process the file line by line */
    while (fgets(line, MAX_LINE_LENGTH, source_fp)) {
        char *ptr = line;
        int isEmpty = TRUE;
        
        /* Skip leading whitespace */
        while (*ptr && (isspace((unsigned char)*ptr))) {
            ptr++;
        }
        
        /* Check if line is empty or comment */
        if (*ptr && *ptr != ';') {
            /* Line contains actual content, write to target file */
            if (fputs(line, target_fp) == EOF) {
                fclose(target_fp);
                print_error_no_line(ERROR_FILE_WRITE);
                return FALSE;
            }
        }
    }
    
    fclose(target_fp);
    return TRUE;
}

/**
 * @brief Processes an assembly file to identify and store macro definitions
 * @param fp File pointer to the assembly source file
 */
void process_as_file(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    char temp_line[MAX_LINE_LENGTH];
    char *token;
    int in_mcro = FALSE;
    int line_number = 0;  
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
                continue;
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
            }
        }
    }

    print_mcro_table(&mcro_table);
    print_mcro_if_called(fp);
    create_am_file(fp, "test.as");
}

