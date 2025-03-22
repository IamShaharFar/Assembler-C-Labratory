# Header_Files

## Overview
The `Header_Files` directory contains essential header files for an assembler project. These headers define key data structures, utility functions, and validation mechanisms used throughout the assembly process. This document provides an overview of each file, its purpose, and how to use them effectively.

## Header Files
Below is a detailed description of each header file included in this directory:

### 1. **utils.h**
- Contains utility functions for string manipulation and validation.
- **Key Functions:**
  - `char* advance_to_next_token(char* str)`: Skips whitespace in a string.
  - `char* advance_past_token(char* str)`: Moves past a token in a string.
  - `int validate_register_operand(const char* str)`: Checks if a string is a valid register operand.
  - `void trim_newline(char *str)`: Trims newline and trailing spaces from a string.
  - `void init_label_table(LabelTable *label_table)`: Initializes a label table.
  - `void init_virtual_pc(VirtualPC *vpc)`: Initializes the virtual PC (VPC) structure.

### 2. **vpc_utils.h**
- Functions for handling Virtual PC (VPC) storage and command processing.
- **Key Functions:**
  - `int process_data_or_string_directive(char *ptr, VirtualPC *vpc, int *storage_full)`: Processes `.data` and `.string` directives and store the values as "words" into the vpc storage.
  - `int process_and_store_command(const char *line, VirtualPC *vpc, int *storage_full)`: Parses and stores assembly commands into "words".
  - `void process_operand(const char *param, unsigned int *first_word, unsigned int *word, int shift_opcode, int shift_reg, int *param_flag)`: Handles operands for assembly instructions and generate the appropriate binary representation.

### 3. **command_utils.h**
- Functions for validating and processing assembly commands.
- **Key Functions:**
  - `int is_valid_command_name(const char *token)`: Validates if a token is a command name.
  - `ErrorCode is_valid_command(const char *line)`: Validates an assembly command.
  - `ErrorCode validate_parameters(char *line, int expected_params, char params[][MAX_LINE_LENGTH])`: Checks if command parameters are valid.

### 4. **errors.h**
- Defines error codes and warning messages for assembly processing.
- **Key Elements:**
  - `typedef enum { ERROR_SUCCESS, ERROR_MEMORY_ALLOCATION, ERROR_FILE_PROCESSING, ERROR_UNKNOWN_COMMAND, ... } ErrorCode;`
  - `void print_error_with_code(ErrorCode code, int line_number, const char *start, const char *end);`
  - `void print_warning(WarningCode code, int line_number);`

### 5. **globals.h**
- Defines global constants and reserved words.
- **Key Definitions:**
  - `#define MAX_LINE_LENGTH 81`
  - `#define TRUE 1`
  - `#define FALSE 0`
  - `extern const char *reserved_words[];`
  - `extern const CommandInfo commands_info[RESERVED_COMMANDS_COUNT];`

### 6. **label_utils.h**
- Functions for label validation and management.
- **Key Functions:**
  - `ErrorCode is_valid_label(const char *label)`: Checks if a label name is valid.
  - `int label_exists(const char *name, LabelTable *label_table)`: Checks if a label exists.
  - `ErrorCode add_label(const char *name, int line_number, const char *line, const char *type, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table)`: Adds a label to the table.

### 7. **output_builder.h**
- Handles output file generation (`.ob`, `.ent`, `.ext`).
- **Key Functions:**
  - `void generate_object_file(const VirtualPC *vpc, const char *filename);`
  - `void generate_entry_file(const LabelTable *label_table, const char *filename);`
  - `void generate_externals_file(const VirtualPC *vpc, const LabelTable *label_table, const char *filename);`

### 8. **preprocessor.h**
- Processes macros and pre-processing directives in assembly files.
- **Key Functions:**
  - `int process_as_file(FILE *fp, const char *file_path, McroTable *mcro_table);`
  - `int process_file(const char* filepath, McroTable *mcro_table);`

### 9. **preprocessor_utils.h**
- Utility functions for macro processing.
- **Key Functions:**
  - `void init_mcro_table(McroTable *table);`
  - `ErrorCode is_valid_mcro_name(const char *name);`
  - `int expand_macros_to_am_file (FILE *source_fp, const char *source_filepath, const McroTable *mcro_table);`

### 10. **first_pass.h**
- Executes the first pass of the assembler to define labels and macros.
- **Key Functions:**
  - `int first_pass(FILE *fp, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table);`

### 11. **first_pass_utils.h**
- Utility functions for the first pass.
- **Key Functions:**
  - `ErrorCode is_data_storage_instruction(char *line);`
  - `int count_data_or_string_elements(char *ptr);`

### 12. **second_pass.h**
- Executes the second pass of the assembler, resolving labels and writing output files.
- **Key Functions:**
  - `int second_pass(FILE *am_file, LabelTable *label_table, VirtualPC *vpc);`
  - `void validate_labels_and_relative_addresses(char *content_after_label, LabelTable *label_table, int line_number, int *is_valid_file);`

## Running the Assembler
1. Ensure all dependencies and header files are correctly included.
2. Compile the assembler using a C compiler (`gcc`).
3. Provide an assembly file (`.as`) as input.
4. The assembler processes the file and generates machine code (`.ob`), entry symbols (`.ent`), and external references (`.ext`).

### Example Run:
```sh
gcc -o assembler main.c -I./Header_Files && ./assembler input.as
```

## Conclusion
This directory serves as the backbone of the assembler project, providing critical utilities, error handling, and structured processing. For modifications or improvements, ensure header dependencies are managed properly.

For further information, refer to the individual header files and their implementation counterparts.

