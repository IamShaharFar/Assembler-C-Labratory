# Source Files Documentation

## Overview
The `Source_Files/` directory contains the core implementation of the assembler. Each source file handles a specific part of the assembly process, from preprocessing to generating output files. Below is a breakdown of each file, including its key functions and role in the assembler.

## Source Files Breakdown

### Entry Point
- **assembler.c**
  - The main entry point of the assembler program.
  - Orchestrates preprocessing, first pass, and second pass.
  - **Key Functions:**
    - `main(int argc, char *argv[])`: Initializes the assembler and handles input files.
    - `delete_file_if_needed(const char *filename, int success)`: Deletes temporary files if necessary.

### Preprocessing
- **preprocessor.c**
  - Handles macro expansion and prepares input files for further processing.
  - **Key Functions:**
    - `process_file(const char *filename, McroTable *mcro_table)`: Validates the provided file path, Ensures proper memory allocation and cleanup and generates .am file.
    - `process_as_file(FILE *fp, const char *file_path, McroTable *mcro_table)`: Processes macros in an assembly file and replaces macro calls with their definitions.
- **preprocessor_utils.c**
  - Utility functions for handling macro definitions.
  - **Key Functions:**
    - `init_mcro_table(McroTable *table)`: Initializes the macro table.
    - `add_mcro(McroTable *table, const char *name)`: Adds a new macro definition.
    - `add_line_to_mcro(McroTable *table, const char *line)`: Appends a line to the last macro definition.
    - `expand_macros_to_am_file(FILE *source_fp, const char *source_filepath, const McroTable *mcro_table, int *is_valid)`: Processes the content as it would appear in the .am file expands macros when called, and removes macro declarations

### First and Second Pass Processing
- **first_pass.c**
  - Parses the assembly file, processes labels, directives, command instructions and detect errors.
  - Utilizes `label_utils.c` for label validation and storing and `command_utils.c` for command validation and processing.
  - **Key Functions:**
    - `first_pass(FILE *fp, VirtualPC *vpc, LabelTable *label_table, const McroTable *mcro_table)`: Executes the first pass over the assembly file.
- **first_pass_utils.c**
  - Helper functions for data and instruction processing during the first pass.
  - **Key Functions:**
    - `is_data_storage_instruction(char *line)`: Checks if a directive is a valid `.data` or `.string`.
    - `count_data_or_string_elements(char *content)`: Counts the number of elements in a data or string directive.
- **second_pass.c**
  - Resolves label addresses and generates the final machine code.
  - **Key Functions:**
    - `second_pass(FILE *am_file, LabelTable *label_table, VirtualPC *vpc)`: Executes the second pass over the assembly file.
    - `validate_labels_and_relative_addresses(const char *line, LabelTable *label_table, int line_number, int *is_valid_file, char *label)`: Validate operands of a command and check for undefined labels

### Output Generation
- **output_builder.c**
  - Generates `.ob`, `.ent`, and `.ext` output files.
  - **Key Functions:**
    - `generate_object_file(VirtualPC *vpc, const char *filename)`: Creates the `.ob` file containing machine code.
    - `generate_entry_file(LabelTable *label_table, const char *filename)`: Generates the `.ent` file for entry labels.
    - `generate_externals_file(VirtualPC *vpc, const LabelTable *label_table, const char *filename)`: Generates the `.ext` file for external references.

### Label and Command Processing
- **label_utils.c**
  - Provides functions for label validation and management.
  - **Key Functions:**
    - `is_valid_label(const char *label)`: Checks if a given label name is valid.
    - `label_exists(const char *name, LabelTable *label_table)`: Checks if a label is already defined.
- **command_utils.c**
  - Validates and processes assembly commands.
  - **Key Functions:**
    - `is_valid_command(const char *line)`: Checks if a command is valid.
    - `get_expected_params(const char *command_name)`: Determines the expected number of parameters for a command.

### Virtual Computer Control
- **vpc_utils.c**
  - Handles memory storage and instruction management within the VirtualPC.
  - **Key Functions:**
    - `process_data_or_string_directive(char *ptr, VirtualPC *vpc, int *storage_full)`: Processes `.data` and `.string` directives and store the values as "words" in the vpc storage.
    - `process_and_store_command(const char *line, VirtualPC *vpc, int *storage_full)`: Converts commands into machine code and stores them as "words" in the vpc storage.

### General Utilities and Error Handling
- **utils.c**
  - Provides general helper functions used throughout the assembler.
  - **Key Functions:**
    - `advance_to_next_token(char *str)`: Skips whitespace to find the next token in a line.
    - `advance_past_token(char *str)`: Advances the pointer past the current token in the string.
    - `advance_past_token_or_comma(char *str)`: Advances the pointer past the current token or comma in the string.
    - `validate_register_operand(const char *str)`: Checks if a string is a valid register operand.
    - `int is_valid_number(const char *s)`: Checks if a string represents a valid integer number.
    - `trim_newline(char *str)`: Trims trailing newline, carriage return, space, and tab characters from a string.
- **errors.c**
  - Defines error and warnings messages and handle reporting.
  - **Key Functions:**
    - `print_error(ErrorCode err, int line_number)`: Prints an error message with a line number.
    - `print_error_no_line(ErrorCode err)`: Prints an error message without a line number.
    - `print_error_with_code(ErrorCode code, int line_number, const char *start, const char *end)`: Prints an error message with the line number, error details, and highlights the erroneous code section.
    - `print_warning(WarningCode code, int line_number)`: Prints a warning message with a line number.
    - `void print_warning_no_line(WarningCode code)`: Prints a warning message without a line number.
- **globals.c**
  - Stores global constants and reserved words used throughout the project.
  - **Key Functions:**
    - `const char *reserved_words[]`: Array of reserved words for the assembler.
    - `const char *commands_names[]`: Array of command names supported by the assembler.
    - `const CommandInfo commands_info[RESERVED_COMMANDS_COUNT]`: Array of commands (name, funct, optcode) supported by the assembler.
