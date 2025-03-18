/* vpc_utils.c */
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/first_pass_utils.h"
#include "../Header_Files/command_utils.h"
#include "../Header_Files/label_utils.h"
#include "../Header_Files/structs.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Processes a .data or .string directive and stores the values in the VirtualPC storage.
 *
 * This function processes a directive from the input string pointed to by `ptr`. It can handle
 * either a .data directive, which contains a list of numbers, or a .string directive, which
 * contains a string of characters. The function stores the processed values in the VirtualPC
 * storage, ensuring that only the lower 24 bits of each value are stored.
 *
 * @param ptr Pointer to the input string containing the directive.
 * @param vpc Pointer to the VirtualPC structure where the values will be stored.
 * @param storage_full Pointer to an integer flag that will be set to if the storage is full.
 * @return The count of processed items (numbers or characters).
 */
int process_data_or_string_directive(char *ptr, VirtualPC *vpc, int *storage_full)
{
    int count = 0; /* Count of the line dc */
    ptr = advance_to_next_token(ptr);

    if (strncmp(ptr, ".data", 5) == 0) /* Check if the directive is .data */
    {
        ptr += 5;
        ptr = advance_to_next_token(ptr);

        /* Iterate over numbers */
        while (*ptr)
        {
            char *endptr;
            int num = strtol(ptr, &endptr, 10); /* Convert number */

            /* Check if the number is valid */
            if (ptr == endptr)
                break;

            if (vpc->DC + vpc->IC < STORAGE_SIZE)
            {
                vpc->storage[vpc->last_adress].value = num & 0xFFFFFF;                                              /* Store only the lower 24 bits of 'num' */
                sprintf(vpc->storage[vpc->last_adress].encoded, "%d", num);                                         /* Store the number as a string */
                vpc->storage[vpc->last_adress].encoded[sizeof(vpc->storage[vpc->last_adress].encoded) - 1] = '\0'; /* Ensure null-termination */
                vpc->last_adress++;
            }
            else
            {
                *storage_full = TRUE;
            }

            count++;
            ptr = endptr; /* Move to the next token */
            ptr = advance_to_next_token(ptr);
            if (*ptr == ',')
            {
                ptr++;
                ptr = advance_to_next_token(ptr);
            }
        }
    }
    else if (strncmp(ptr, ".string", 7) == 0) /* Check if the directive is .string */
    {
        ptr += 7;
        ptr = advance_to_next_token(ptr);

        /* Check for valid string */
        if (*ptr == '"')
        {
            ptr++;

            /* Iterate over characters in the string */
            while (*ptr && *ptr != '"')
            {
                if (vpc->DC + vpc->IC < STORAGE_SIZE)
                {
                    vpc->storage[vpc->last_adress].value = (int)(*ptr) & 0xFFFFFF; /* Store only the lower 24 bits of the character */
                    sprintf(vpc->storage[vpc->last_adress].encoded, "%c", *ptr);   /* Store the character as a string */
                    vpc->last_adress++;
                }
                else
                {
                    *storage_full = TRUE;
                }

                count++;
                ptr++;
            }

            if (vpc->DC + vpc->IC < STORAGE_SIZE)
            {
                vpc->storage[vpc->last_adress].value = 0; /* Store null-terminator */
                vpc->last_adress++;
            }
            else
            {
                *storage_full = TRUE;
            }

            count++;
        }
    }
    return count;
}

/**
 * @brief Generates words from a command from a valid line of command and stores it in the VirtualPC storage.
 *
 * This function processes a valid line of command from the input string `line`. It extracts the command
 * and its parameters, determines the opcode and function code, and generates the corresponding binary
 * representation. The command and its parameters are then stored in the VirtualPC storage.
 *
 * @param line Pointer to the input string containing the command line.
 * @param vpc Pointer to the VirtualPC structure where the binary command will be stored.
 * @param storage_full Pointer to an integer flag that will be set to if the storage is full.
 * @return The count of stored items (1 for the command itself, plus any additional parameters).
 */
int process_and_store_command(const char *line, VirtualPC *vpc, int *storage_full)
{
    char command[MAX_LINE_LENGTH];
    char param1[MAX_LINE_LENGTH] = "";
    char param2[MAX_LINE_LENGTH] = "";
    char *modifiable_line;
    char *ptr;
    int expected_params, i;
    unsigned int first_word = 0;
    unsigned int second_word = 0;
    unsigned int third_word = 0;
    int param_flags[2] = {0, 0}; /* Flags to indicate if a parameter generates a word */
    int opcode = -1, funct = -1;

    /* Make a modifiable copy of the input line */
    modifiable_line = malloc(strlen(line) + 1);
    if (!modifiable_line)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return 0;
    }
    strcpy(modifiable_line, line);

    /* Extract command name */
    ptr = advance_to_next_token(modifiable_line);
    sscanf(ptr, "%s", command);
    ptr = advance_past_token(ptr);
    ptr = advance_to_next_token(ptr);

    /* Determine expected parameters */
    expected_params = get_expected_params(command);

    /* Extract parameters based on expected count */
    if (expected_params == 1)
    {
        sscanf(ptr, "%s", param1);
    }
    else if (expected_params == 2)
    {
        sscanf(ptr, "%[^, ]", param1); /* Read param1 until space or comma */
        ptr = advance_past_token_or_comma(ptr);
        ptr = advance_to_next_token(ptr);
        ptr++;
        sscanf(ptr, "%s", param2); /* Read param2 */
    }

    /* Find the command opcode and funct */
    for (i = 0; i < RESERVED_COMMANDS_COUNT; i++)
    {
        if (strcmp(commands_info[i].name, command) == 0)
        {
            opcode = commands_info[i].opcode;
            funct = commands_info[i].funct;
            break;
        }
    }

    /* Check if the command is valid */
    if (opcode == -1)
    {
        print_error_no_line(ERROR_UNKNOWN_COMMAND);
        free(modifiable_line);
        return 0;
    }

    /* Process parameters and add a word to vpc if needed */
    if (expected_params == 1)
    {
        process_operand(param1, &first_word, &second_word, 11, 8, &param_flags[0]);
    }
    else if (expected_params == 2)
    {
        process_operand(param1, &first_word, &second_word, 16, 13, &param_flags[0]);
        process_operand(param2, &first_word, &third_word, 11, 8, &param_flags[1]);
    }

    /* Set bit 2 to 1 */
    first_word |= (1 << 2);

    /* Set bits 18-23 for opcode */
    first_word |= (opcode & 0x3F) << 18; /* Opcode is 6 bits, mask with 0x3F (binary 111111) and shift left by 18 bits */

    /* Set bits 3-7 for funct */
    if (funct != -1)
    {
        /* Set bits 3-7 for funct */
        first_word |= (funct & 0x1F) << 3; /* Funct is 5 bits, mask with 0x1F (binary 11111) and shift left by 3 bits */
    }

    /* Store the words in the VirtualPC storage */
    if (vpc->IC < STORAGE_SIZE)
    {
        vpc->storage[vpc->IC].value = first_word;
        strncpy(vpc->storage[vpc->IC].encoded, command, sizeof(vpc->storage[vpc->IC].encoded) - 1); /* Store the command name that gave a word*/
        vpc->storage[vpc->IC].encoded[sizeof(vpc->storage[vpc->IC].encoded) - 1] = '\0';
        vpc->IC++;
        vpc->last_adress++;
    }
    else
    {
        *storage_full = TRUE;
    }

    if (param_flags[0])
    {
        if (vpc->IC < STORAGE_SIZE)
        {
            vpc->storage[vpc->IC].value = second_word;
            strncpy(vpc->storage[vpc->IC].encoded, param1, sizeof(vpc->storage[vpc->IC].encoded) - 1); /* Store the first parameter that gave a word */
            vpc->storage[vpc->IC].encoded[sizeof(vpc->storage[vpc->IC].encoded) - 1] = '\0';
            vpc->IC++;
            vpc->last_adress++;
        }
        else
        {
            *storage_full = TRUE;
        }
    }
    if (param_flags[1])
    {
        if (vpc->IC < STORAGE_SIZE)
        {
            vpc->storage[vpc->IC].value = third_word;
            strncpy(vpc->storage[vpc->IC].encoded, param2, sizeof(vpc->storage[vpc->IC].encoded) - 1); /* Store the second parameter that gave a word */
            vpc->storage[vpc->IC].encoded[sizeof(vpc->storage[vpc->IC].encoded) - 1] = '\0';
            vpc->IC++;
            vpc->last_adress++;
        }
        else
        {
            *storage_full = TRUE;
        }
    }

    free(modifiable_line); 
    return 1 + param_flags[0] + param_flags[1]; /* Return the count of stored words */
}

/**
 * @brief Processes an operand and updates the binary representation in the provided words.
 *
 * This function processes an operand from the input string `param`. It determines the addressing mode
 * (immediate, relative, direct, or register), updates the `first_word` and `word` with the appropriate
 * binary representation, and sets the `param_flag` if the operand is valid.
 *
 * @param param Pointer to the input string containing the operand.
 * @param first_word Pointer to the first word of the binary representation to be updated.
 * @param word Pointer to the word where the operand value will be stored.
 * @param shift_opcode Number of bits to shift the opcode for the addressing mode.
 * @param shift_reg Number of bits to shift the register number.
 * @param param_flag Pointer to the flag indicating if the operand is valid.
 */
void process_operand(const char *param, unsigned int *first_word, unsigned int *word, int shift_opcode, int shift_reg, int *param_flag)
{
    int value;

    if (!validate_register_operand(param))
    {
        *param_flag = 1;

        if (param[0] == '#')
        {
            *first_word |= (0 & 0x03) << shift_opcode; /* Immediate addressing - 0 */
            *first_word |= (0 & 0x07) << shift_reg;  /* No register */
            value = atoi(&param[1]); /* Convert number after '#' */

            if (value < 0)
            {
                value = (1 << 21) + value; /* Two's complement for 21-bit signed numbers */
            }

            *word |= (value & 0x1FFFFF) << 3; /* Mask the value to 21 bits (0x1FFFFF, which is 21 bits set to 1) and store it in bits 3-23 of the word */
            *word |= (1 << 2);                /* Set bit 2 to 1 */
        }
        else if (param[0] == '&')
        {
            *first_word |= (2 & 0x03) << shift_opcode; /* Relative addressing - 2*/
            *first_word |= (0 & 0x07) << shift_reg;  /* No register */
            *word |= (1 << 2); /* Set bit 2 to 1 */
        }
        else
        {
            *first_word |= (1 & 0x03) << shift_opcode; /* Direct addressing - 1 */
            *first_word |= (0 & 0x07) << shift_reg; /* No register */
            *word = 0; /* Clear the word */
            *word |= 0x03; /* Set bits 0-1 to 1 (11 in binary) in the second pass we will update the address and the R/E*/
        }
    }
    else
    {
        int reg_num = atoi(&param[1]); /* Extract register number */
        *first_word |= (reg_num & 0x07) << shift_reg; /* Set the register bits with the register number */
        *first_word |= (3 & 0x03) << shift_opcode; /* Register addressing - 3 */
    }
}

/**
 * @brief Resolves and updates words in VirtualPC storage with label addresses.
 *
 * This function scans the VirtualPC storage from address 100 to (IC + DC),
 * checking if a word's `encoded` field starts with '&' (indicating relative addressing)
 * or contains a label from the label table. It then updates the word's value with the
 * appropriate address or relative offset.
 *
 * @param vpc Pointer to the VirtualPC structure.
 * @param label_table Pointer to the LabelTable structure.
 */
int resolve_and_update_labels(VirtualPC *vpc, const LabelTable *label_table)
{
    /* Initialize start and end addresses for scanning the VirtualPC storage */
    uint32_t start_addr = 100;
    uint32_t end_addr = vpc->IC + vpc->DC;

    /* Loop variables and temporary storage for label address and value */
    int i, j;
    int label_address = 0;
    int value = 0;
    int is_valid = TRUE;

    if (vpc == NULL || label_table == NULL)
    {
        is_valid = FALSE;
        print_error_no_line(ERROR_NULL_POINTER);
        return is_valid;
    }

    /* Loop through VirtualPC storage to resolve and update label addresses */
    for (i = start_addr; i < end_addr; i++)
    {
        char *encoded_str = vpc->storage[i].encoded;
        int32_t word_value = vpc->storage[i].value;
        label_address = 0;
        value = 0;

        /* Check if the given encoded string indicates a label passed by Relative addressing.
         * If true, the function will calculate the distance from the current address to the label address. */
        if (encoded_str[0] == '&')
        {
            for (j = 0; j < label_table->count; j++)
            {
                if (strcmp(&encoded_str[1], label_table->labels[j].name) == 0)
                {
                    label_address = label_table->labels[j].address;
                    /* Calculate relative address (-1 to reach command address) */
                    value = label_address - (i - 1);

                    word_value &= ~(0x1FFFFF << 3);        /* Clear bits 3-23 */
                    word_value |= (value & 0x1FFFFF) << 3; /* Set bits 3-23 with the value*/
                    vpc->storage[i].value = word_value;

                    break;
                }
            }
            continue;
        }

        for (j = 0; j < label_table->count; j++)
        {
            /* check if the encoded string is a label */
            if (strcmp(encoded_str, label_table->labels[j].name) == 0)
            {
                label_address = label_table->labels[j].address;
                value = label_address;

                vpc->storage[i].value = 0;                        /* Clear the value */
                vpc->storage[i].value |= (value & 0x1FFFFF) << 3; /* Set bits 3-23 with the value*/
                vpc->storage[i].value &= ~(1 << 2);               /* Set bit 2 to 0 */

                /* Set the E/R bits based on the label type */
                if (strcmp(label_table->labels[j].type, "external") == 0)
                {
                    vpc->storage[i].value |= (1 << 0);  /* Set bit 0 to 1 */
                    vpc->storage[i].value &= ~(1 << 1); /* Set bit 1 to 0 */
                }
                else
                {
                    vpc->storage[i].value &= ~(1 << 0); /* Set bit 0 to 0 */
                    vpc->storage[i].value |= (1 << 1);  /* Set bit 1 to 1 */
                }

                break;
            }
        }
    }
    return is_valid;
}