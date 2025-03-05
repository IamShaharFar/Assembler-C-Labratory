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

int add_word_to_vpc(VirtualPC *vpc, Word word)
{
    if (vpc->IC >= STORAGE_SIZE)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return 0;
    }

    vpc->storage[vpc->IC] = word;
    vpc->IC++;
    return 1;
}

/**
 * @brief Converts an integer to a 24-bit binary string.
 *
 * @param num The integer to convert.
 * @param binary_str A buffer to store the 24-bit binary string.
 */
void int_to_binary_24(int num, char binary_str[25])
{
    int i;
    for (i = 23; i >= 0; i--)
    {
        binary_str[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }
    binary_str[24] = '\0';
}

/**
 * @brief Processes .data or .string directive and stores binary words in VirtualPC.
 *
 * @param ptr Pointer to the .data or .string. line content.
 * @param vpc Pointer to the VirtualPC structure.
 * @return int The count of binary words stored.
 */
int process_data_or_string_directive(char *ptr, VirtualPC *vpc)
{
    int count = 0; /* Count of the line dc */
    ptr = advance_to_next_token(ptr);

    if (strncmp(ptr, ".data", 5) == 0)
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
                vpc->storage[vpc->DC + vpc->IC].value = num & 0xFFFFFF;                                              /* Store the number in bits 0-23 */
                sprintf(vpc->storage[vpc->DC + vpc->IC].encoded, "%d", num);                                         /* Store the number as a string */
                vpc->storage[vpc->DC + vpc->IC].encoded[sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1] = '\0'; /* Ensure null-termination */
                vpc->DC++;
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
    else if (strncmp(ptr, ".string", 7) == 0)
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
                    vpc->storage[vpc->DC + vpc->IC].value = (int)(*ptr) & 0xFFFFFF; /* Store the character in bits 0-23 */
                    sprintf(vpc->storage[vpc->DC + vpc->IC].encoded, "%c", *ptr);   /* Store the character as a string */
                    vpc->DC++;
                }

                count++;
                ptr++;
            }

            if (vpc->DC + vpc->IC < STORAGE_SIZE)
            {
                vpc->storage[vpc->DC + vpc->IC].value = 0; /* Store null-terminator */
                vpc->DC++;
            }

            count++;
        }
    }
    return count;
}

/**
 * Generates a binary command from the given assembly line and stores it in the VirtualPC structure.
 *
 * @param line The assembly line to be converted into a binary command.
 * @param vpc A pointer to the VirtualPC structure where the binary command will be stored.
 * @return An integer indicating the success or failure of the operation.
 */

int generate_binary_command(const char *line, VirtualPC *vpc)
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
    int param_flags[2] = {0, 0};
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
        sscanf(ptr, "%s", param2);
    }

    /* Find the command opcode */
    for (i = 0; i < RESERVED_COMMANDS_COUNT; i++)
    {
        if (strcmp(commands_info[i].name, command) == 0)
        {
            opcode = commands_info[i].opcode;
            funct = commands_info[i].funct;
            break;
        }
    }

    if (opcode == -1)
    {
        print_error_no_line(ERROR_UNKNOWN_COMMAND);
        free(modifiable_line);
        return 0;
    }

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
    first_word |= (opcode & 0x3F) << 18; /* Opcode is 6 bits */

    if (funct != -1)
    {
        first_word |= (funct & 0x1F) << 3; /* Funct is 5 bits */
    }
    if (vpc->DC + vpc->IC < STORAGE_SIZE)
    {
        vpc->storage[vpc->DC + vpc->IC].value = first_word;
        strncpy(vpc->storage[vpc->DC + vpc->IC].encoded, command, sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1);
        vpc->storage[vpc->DC + vpc->IC].encoded[sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1] = '\0';
        vpc->IC++;
    }
    if (param_flags[0])
    {
        if (vpc->DC + vpc->IC < STORAGE_SIZE)
        {
            vpc->storage[vpc->DC + vpc->IC].value = second_word;
            strncpy(vpc->storage[vpc->DC + vpc->IC].encoded, param1, sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1);
            vpc->storage[vpc->DC + vpc->IC].encoded[sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1] = '\0';
            vpc->IC++;
        }
    }
    if (param_flags[1])
    {
        if (vpc->DC + vpc->IC < STORAGE_SIZE)
        {
            vpc->storage[vpc->DC + vpc->IC].value = third_word;
            strncpy(vpc->storage[vpc->DC + vpc->IC].encoded, param2, sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1);
            vpc->storage[vpc->DC + vpc->IC].encoded[sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1] = '\0';
            vpc->IC++;
        }
    }
    free(modifiable_line);
    return 1 + param_flags[0] + param_flags[1];
}

void process_operand(const char *param, unsigned int *first_word, unsigned int *word, int shift_opcode, int shift_reg, int *param_flag)
{
    int value;

    if (!validate_register_operand(param))
    {
        *param_flag = 1;

        if (param[0] == '#')
        {
            *first_word |= (0 & 0x03) << shift_opcode; /* Immediate addressing */
            *first_word |= (0 & 0x07) << shift_reg;
            value = atoi(&param[1]); /* Convert number after '#' */

            if (value < 0)
            {
                value = (1 << 21) + value; /* Two's complement for 21-bit signed numbers */
            }

            *word |= (value & 0x1FFFFF) << 3; /* Store value in bits 3-23 */
            *word |= (1 << 2);                /* Set bit 2 to 1 */
        }
        else if (param[0] == '&')
        {
            *first_word |= (2 & 0x03) << shift_opcode; /* Relative addressing */
            *first_word |= (0 & 0x07) << shift_reg;
            *word |= (1 << 2); /* Set bit 2 to 1 */
        }
        else
        {
            *first_word |= (1 & 0x03) << shift_opcode; /* Direct addressing */
            *first_word |= (0 & 0x07) << shift_reg;
            *word = 0;
            *word |= 0x03; /* Set bits 0-1 to 1 (0b11) */
        }
    }
    else
    {
        int reg_num = atoi(&param[1]); /* Extract register number */
        *first_word |= (reg_num & 0x07) << shift_reg;
        *first_word |= (3 & 0x03) << shift_opcode; /* Register addressing */
    }
}

void print_virtual_pc_memory(const VirtualPC *vpc)
{
    char binary_str[25];
    int address;

    printf("Virtual PC Memory Dump:\n");
    printf("IC: %d - DC: %d\n", vpc->IC - 100, vpc->DC);
    printf("---------------------------------------------------------------\n");
    printf("| Address  | Binary Value           | Encoded String           |\n");
    printf("---------------------------------------------------------------\n");

    for (address = 100; address < (vpc->DC + vpc->IC); address++)
    {
        int value = vpc->storage[address].value;
        int_to_binary_24(value, binary_str);

        printf("| %-8d | %-24s | %-24s |\n",
               address, binary_str, vpc->storage[address].encoded);
    }

    printf("---------------------------------------------------------------\n");
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
