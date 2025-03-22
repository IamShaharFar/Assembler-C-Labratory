# Header Files Documentation

## Overview
The `Header_Files/` directory contains the header files that define the core structures, constants, and function prototypes used throughout the assembler project. These headers ensure modularity, maintainability, and separation of concerns within the codebase.

## Header Files
Below is a breakdown of the header files and their purposes:

### Core Headers
- **globals.h**: Defines global constants and reserved words used across multiple modules.
- **structs.h**: Defines data structures such as `VirtualPC`, `Label`, `Mcro`, and `CommandInfo`.

### Error Handling
- **errors.h**: Contains error codes and functions for handling error messages and reporting issues encountered during assembly.

### Preprocessing
- **preprocessor.h**: Declares functions related to macro expansion and source file preprocessing.
- **preprocessor_utils.h**: Provides utility functions to assist with macro handling.

### First and Second Pass Processing
- **first_pass.h**: Declares functions for parsing assembly files and identifying labels, directives, and commands.
- **first_pass_utils.h**: Utility functions for data storage processing in the first pass.
- **second_pass.h**: Defines functions for resolving label addresses, handling entry directives, and generating final machine code.

### Label and Command Processing
- **label_utils.h**: Functions for managing labels, checking their validity, and handling label-related errors.
- **command_utils.h**: Provides functions for validating and processing assembly commands.

### Output Generation
- **output_builder.h**: Declares functions for generating `.ob`, `.ent`, and `.ext` output files based on successful assembly.

### Virtual Program Control
- **vpc_utils.h**: Functions for managing the `VirtualPC` structure, handling memory, and storing machine instructions.

### General Utilities
- **utils.h**: Provides utility functions for string manipulation, memory management, and general helper operations.

<<<<<<< HEAD
### 9. **preprocessor_utils.h**
- Utility functions for macro processing.
- **Key Functions:**
  - `void init_mcro_table(McroTable *table);`
  - `ErrorCode is_valid_mcro_name(const char *name);`
  - `int expand_macros_to_am_file (FILE *source_fp, const char *source_filepath, const McroTable *mcro_table);`
=======
## Usage
These header files should be included in the corresponding `Source_Files/` implementations to ensure proper function declaration and structure usage. They should not contain function definitions but only prototypes, macros, and data structure declarations.
>>>>>>> ac6ce29c78c35619f6bb0735ba26d15ceaa7f504

## Contact
For further inquiries, reach out to the project maintainers or consult the main `README.md` for general project details.

