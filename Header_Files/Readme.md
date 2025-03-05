# Headers Directory

## Overview

The `Headers` directory contains all the header files for the assembler project. These header files define the interfaces and data structures used throughout the project. Each header file corresponds to a specific module or functionality within the assembler. This directory is crucial for maintaining modularity and organization in the project.

## File Descriptions

### `command_utils.h`
- **Description**: Header file for command validation and processing functions in the assembler.
- **Key Functions**:
  - `is_valid_command_name`: Checks if a given token is a valid command name.
  - `is_valid_command`: Validates a given assembly command.
  - `get_expected_params`: Determines the number of parameters expected for a given command.
  - `skip_command_name`: Moves the pointer past the command name in a given line.
  - `validate_parameters`: Validates the parameters of a command based on the expected count.
  - `validate_command_params`: Validates the specific parameters for a given command.

### `errors.h`
- **Description**: Header file for error handling in the assembler.
- **Key Components**:
  - `ErrorCode`: Enumeration of various error codes.
  - `Error`: Structure representing an error with its code, name, and message.
  - `print_error`: Prints an error message with the line number.
  - `print_error_no_line`: Prints an error message without the line number.
  - `get_error_message`: Retrieves the error message for a given error code.
  - `get_error_name`: Retrieves the error name for a given error code.

### `first_pass_utils.h`
- **Description**: Header file for utility functions used in the first pass of the assembler.
- **Key Functions**:
  - `is_data_storage_instruction`: Checks if a given line is a valid `.data` or `.string` directive and validates its format.
  - `count_data_or_string_elements`: Counts elements in a `.data` or `.string` directive.

### `first_pass.h`
- **Description**: Header file for the first pass of the assembler.
- **Key Functions**:
  - `first_pass`: Processes the assembly file for the first pass to identify labels.
  - `print_labels`: Prints all labels stored in the label table.

### `globals.h`
- **Description**: Header file for global constants and variables used throughout the assembler.
- **Key Components**:
  - Constants for maximum lengths and sizes.
  - External declarations for reserved words and command names.
  - `CommandInfo`: Structure representing command information.

### `label_utils.h`
- **Description**: Header file for label utility functions in the assembler.
- **Key Functions**:
  - `is_valid_label`: Validates whether a given string is a valid label name.
  - `label_exists`: Checks whether a label exists in the label table.
  - `is_valid_extern_label`: Validates an `.extern` directive and extracts the label.
  - `is_valid_entry_label`: Checks if a given line is a valid `.entry` directive.
  - `is_valid_entry_or_extern_line`: Checks if a line is a valid `.entry` or `.extern` directive.
  - `add_label`: Adds a new label to the label table.
  - `print_mcro_names`: Prints the names of all macros in the given `McroTable`.

### `output_builder.h`
- **Description**: Header file for functions that generate output files from the assembled machine code.
- **Key Functions**:
  - `generate_object_file`: Writes the assembled machine code into a `.ob` file.
  - `generate_entry_file`: Writes the entry labels into a `.ent` file.
  - `generate_externals_file`: Writes the external labels into a `.ext` file.
  - `compare_labels_by_address`: Comparison function for sorting labels by address.

### `preprocessor_utils.h`
- **Description**: Header file for utility functions used in the preprocessor module.
- **Key Functions**:
  - `init_mcro_table`: Initializes the macro table to an empty state.
  - `is_valid_mcro_name`: Validates if a given name is a legal macro name.
  - `add_mcro`: Adds a new macro to the macro table.
  - `add_line_to_mcro`: Adds a line of content to the most recently added macro.
  - `print_mcro_table`: Prints all macros stored in the macro table.
  - `create_am_file`: Processes the content as it would appear in the `.am` file and prints it to stdout.

### `preprocessor.h`
- **Description**: Header file for the preprocessor module.
- **Key Functions**:
  - `process_as_file`: Processes the macros in the given assembly file.
  - `process_file`: Processes the given assembly file.
  - `create_am_file`: Saves the processed content as a `.am` file.

### `second_pass.h`
- **Description**: Header file for the second pass of the assembler.
- **Key Functions**:
  - `second_pass`: Processes an assembly source file in its second pass.
  - `validate_labels_and_relative_addresses`: Processes a command line to validate operands and check for undefined labels.

### `structs.h`
- **Description**: Header file defining the data structures used in the assembler.
- **Key Structures**:
  - `Word`: Represents a single word in the virtual PC.
  - `VirtualPC`: Represents the virtual PC with storage and counters.
  - `CommandInfo`: Represents command information.
  - `Mcro`: Represents a macro with its name and content lines.
  - `McroTable`: Holds all the macros defined in the source file.
  - `Label`: Represents a label with its name, line number, and the full line content.
  - `LabelTable`: Holds a list of labels found in the source file.

### `utils.h`
- **Description**: Header file for general utility functions used throughout the assembler.
- **Key Functions**:
  - `advance_to_next_token`: Advances pointer past whitespace characters.
  - `advance_past_token`: Advances the pointer past the current token.
  - `advance_past_token_or_comma`: Advances the pointer past the current token until it hits a space, tab, or comma.
  - `validate_register_operand`: Validates register operand format.
  - `check_line_length_exceeded`: Checks if line length exceeds the maximum allowed length.
  - `detect_consecutive_commas`: Detects multiple commas between tokens.
  - `verify_command_end`: Ensures no extra tokens after command.
  - `trim_newline`: Trims newline characters from a string.
  - `init_label_table`: Initializes the label table.
  - `init_virtual_pc`: Initializes the virtual PC.

### `vpc_utils.h`
- **Description**: Header file for utility functions related to the virtual PC.
- **Key Functions**:
  - `add_word_to_vpc`: Adds a word to the virtual PC at the current PC address.
  - `process_data_or_string_directive`: Processes `.data` or `.string` directive and stores binary words in the virtual PC.
  - `int_to_binary_24`: Converts an integer to a 24-bit binary string.
  - `generate_binary_command`: Generates binary code for a command.
  - `process_operand`: Processes an operand and updates the virtual PC.
  - `print_virtual_pc_memory`: Prints the memory of the virtual PC.
  - `resolve_and_update_labels`: Resolves and updates words in the virtual PC storage with label addresses.

## Conclusion

The header files in the `Headers` directory are essential for defining the interfaces and data structures used throughout the assembler project. They ensure modularity, maintainability, and clarity in the codebase. Each header file corresponds to a specific module or functionality, making it easier to manage and understand the project.

This README provides a comprehensive overview of the header files and their key functions, making it suitable for submission as a final project to the university.