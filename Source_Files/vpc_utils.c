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
 * @brief Prints the binary representation of each element in a valid .data or .string directive
 *        and returns the count of printed binary words.
 *
 * @param ptr Pointer to the content after .data or .string.
 * @return int The count of binary words printed.
 */
int print_data_or_string_binary(char *ptr, int address, VirtualPC *vpc)
{
    char binary_str[25];
    int count = 0;
    ptr = advance_to_next_token(ptr);

    if (strncmp(ptr, ".data", 5) == 0)
    {
        ptr += 5;
        ptr = advance_to_next_token(ptr);

        while (*ptr)
        {
            char *endptr;
            int num = strtol(ptr, &endptr, 10);
            if (ptr == endptr)
                break;

            int_to_binary_24(num, binary_str);
            /* Store binary word in VirtualPC storage */
            if (vpc->DC + vpc->IC < STORAGE_SIZE)
            {
                vpc->storage[vpc->DC + vpc->IC].value = num & 0xFFFFFF; /* Ensure 24-bit storage */
                sprintf(vpc->storage[vpc->DC + vpc->IC].encoded, "%d", num);
                vpc->storage[vpc->DC + vpc->IC].encoded[sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1] = '\0';
                vpc->DC++;
            }

            count++;
            ptr = endptr;
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

        if (*ptr == '"')
        {
            ptr++;
            while (*ptr && *ptr != '"')
            {
                int_to_binary_24((int)(*ptr), binary_str);
                /* printf("address: %d - %s\n", address + count, binary_str); */

                /* Store character as binary in VirtualPC storage */
                if (vpc->DC + vpc->IC < STORAGE_SIZE)
                {
                    vpc->storage[vpc->DC + vpc->IC].value = (int)(*ptr) & 0xFFFFFF;
                    sprintf(vpc->storage[vpc->DC + vpc->IC].encoded, "%c", *ptr);
                    vpc->DC++;
                }

                count++;
                ptr++;
            }
            /* Encode and print the null terminator ('\0') */
            int_to_binary_24(0, binary_str);
            /* printf("address: %d - %s\n", address + count, binary_str); */

            /* Store null terminator in VirtualPC storage */
            if (vpc->DC + vpc->IC < STORAGE_SIZE)
            {
                vpc->storage[vpc->DC + vpc->IC].value = 0;
                vpc->DC++;
            }

            count++;
        }
    }
    /* print_virtual_pc_memory(vpc); */
    return count;
}

void print_binary(unsigned int num, int bits)
{
    int i;
    for (i = bits - 1; i >= 0; i--)
    {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");
}

int generate_binary_command(const char *line, int address, VirtualPC *vpc)
{
    char command[MAX_LINE_LENGTH];
    char param1[MAX_LINE_LENGTH] = "";
    char param2[MAX_LINE_LENGTH] = "";
    char *modifiable_line;
    char *ptr;
    int expected_params, i, value;
    unsigned int first_word = 0;
    unsigned int second_word = 0;
    unsigned int third_word = 0;
    int param_flags[2] = {0, 0};
    int opcode = -1, funct = -1;

    /* Make a modifiable copy of the input line */
    modifiable_line = malloc(strlen(line) + 1);
    if (!modifiable_line)
    {
        printf("Error: Memory allocation failed!\n");
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
        printf("Error: Unknown command name!\n");
        free(modifiable_line);
        return 0;
    }

    if (expected_params == 1)
    {
        if (!validate_register_operand(param1))
        {
            param_flags[0] = 1;
            if (param1[0] == '#')
            {
                first_word |= (0 & 0x03) << 11; /* Set bits 11-12 to 0 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
                value = atoi(&param1[1]);       /* Convert number after '#' */
                /* Convert to two's complement if negative */
                if (value < 0)
                {
                    value = (1 << 21) + value; /* Two's complement for 21-bit signed numbers */
                }

                /* Store value in bits 3-23 */
                second_word |= (value & 0x1FFFFF) << 3;
                second_word |= (1 << 2); /* Set bit 2 to 1 */
            }
            else if (param1[0] == '&')
            {
                first_word |= (2 & 0x03) << 11; /* Set bits 11-12 to 2 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
                /* TODO : handle distance between current adress to the label adress and code it into bits 3-23*/
                second_word |= (1 << 2); /* Set bit 2 to 1 */
            }
            else
            {
                first_word |= (1 & 0x03) << 11; /* Set bits 11-12 to 1 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
                /* TODO : get the adress and code the word according */
                second_word = 0;
                second_word |= 0x03; /* Set bits 0-1 to 1 (0b11) */
            }
        }
        else
        {
            int reg_num = atoi(&param1[1]);      /* Extract the number after 'r' */
            first_word |= (reg_num & 0x07) << 8; /* Set bits 8-10 */
            first_word |= (3 & 0x03) << 11;      /* Set bits 11-12 to 3 (binary 11) */
        }
    }
    else if (expected_params == 2)
    {
        if (!validate_register_operand(param1))
        {
            param_flags[0] = 1;
            if (param1[0] == '#')
            {
                first_word |= (0 & 0x03) << 16; /* Set bits 16-17 to 0 */
                first_word |= (0 & 0x07) << 13; /* Set bits 13-15 to 0 */
                value = atoi(&param1[1]);       /* Convert number after '#' */
                /* Convert to two's complement if negative */
                if (value < 0)
                {
                    value = (1 << 21) + value; /* Two's complement for 21-bit signed numbers */
                }

                /* Store value in bits 3-23 */
                second_word |= (value & 0x1FFFFF) << 3;
                second_word |= (1 << 2); /* Set bit 2 to 1 */
            }
            else if (param1[0] == '&')
            {
                first_word |= (2 & 0x03) << 16; /* Set bits 16-17 to 2 */
                first_word |= (0 & 0x07) << 13; /* Set bits 13-15 to 0 */
                /* TODO : handle distance between current adress to the label adress and code it into bits 3-23*/
                second_word |= (1 << 2); /* Set bit 2 to 1 */
            }
            else
            {
                first_word |= (1 & 0x03) << 16; /* Set bits 16-17 to 1 */
                first_word |= (0 & 0x07) << 13; /* Set bits 13-15 to 0 */
                /* TODO : get the adress and code the word according */
                second_word = 0;
                second_word |= 0x03; /* Set bits 0-1 to 1 (0b11) */
            }
        }
        else
        {
            int reg_num = atoi(&param1[1]);       /* Extract the number after 'r' */
            first_word |= (reg_num & 0x07) << 13; /* Set bits 13-15 */
            first_word |= (3 & 0x03) << 16;       /* Set bits 16-17 to 3 (binary 11) */
        }

        if (!validate_register_operand(param2))
        {
            param_flags[1] = 1;
            if (param2[0] == '#')
            {
                first_word |= (0 & 0x03) << 11; /* Set bits 11-12 to 0 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
                value = atoi(&param2[1]);       /* Convert number after '#' */

                /* Convert to two's complement if negative */
                if (value < 0)
                {
                    value = (1 << 21) + value; /* Two's complement for 21-bit signed numbers */
                }

                /* Store value in bits 3-23 */
                third_word |= (value & 0x1FFFFF) << 3;
                third_word |= (1 << 2); /* Set bit 2 to 1 */
            }
            else if (param2[0] == '&')
            {
                first_word |= (2 & 0x03) << 11; /* Set bits 11-12 to 2 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
                /* TODO : handle distance between current adress to the label adress and code it into bits 3-23*/
                third_word |= (1 << 2); /* Set bit 2 to 1 */
            }
            else
            {
                first_word |= (1 & 0x03) << 11; /* Set bits 11-12 to 2 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
                /* TODO : get the adress and code the word according */
                third_word = 0;
                third_word |= 0x03; /* Set bits 0-1 to 1 (0b11) */
            }
        }
        else
        {
            int reg_num = atoi(&param2[1]);      /* Extract the number after 'r' */
            first_word |= (reg_num & 0x07) << 8; /* Set bits 8-10 */
            first_word |= (3 & 0x03) << 11;      /* Set bits 11-12 to 3 (binary 11) */
        }
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
    /* printf("address: %d - %s - ", address, command);
    print_binary(first_word, 24); */
    if (param_flags[0])
    {
        if (vpc->DC + vpc->IC < STORAGE_SIZE)
        {
            vpc->storage[vpc->DC + vpc->IC].value = second_word;
            strncpy(vpc->storage[vpc->DC + vpc->IC].encoded, param1, sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1);
            vpc->storage[vpc->DC + vpc->IC].encoded[sizeof(vpc->storage[vpc->DC + vpc->IC].encoded) - 1] = '\0';
            vpc->IC++;
        }
        /* printf("address: %d - %s - ", address + param_flags[0], param1);
        print_binary(second_word, 24); */
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
        /* printf("address: %d - %s - ", address + param_flags[0] + param_flags[1], param2);
        print_binary(third_word, 24);*/
    }
    free(modifiable_line);
    return 1 + param_flags[0] + param_flags[1];
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
 * @brief Prints words where the encoded string starts with '&' or matches a label from the label table.
 *
 * This function scans the VirtualPC storage from address 100 to (100 + IC + DC),
 * checking if a word's `encoded` field starts with '&' (indicating relative addressing)
 * or contains a label from the label table.
 *
 * @param vpc Pointer to the VirtualPC structure.
 * @param label_table Pointer to the LabelTable structure.
 */
void print_words_with_labels(VirtualPC *vpc, const LabelTable *label_table)
{
    uint32_t start_addr = 100;
    uint32_t end_addr = 100 + vpc->IC + vpc->DC;
    int i, j;
    int label_address = 0;
    int value = 0;

    if (vpc == NULL || label_table == NULL)
    {
        fprintf(stderr, "Error: NULL pointer passed to print_words_with_labels.\n");
        return;
    }

    printf("Words with Labels or Relative Addressing:\n");
    printf("--------------------------------------------------------------------------\n");
    printf("| Address | Encoded String       | Binary Value  | Value                  |\n");
    printf("--------------------------------------------------------------------------\n");

    /* Scan through VirtualPC storage */
    for (i = start_addr; i < end_addr; i++)
    {
        char *encoded_str = vpc->storage[i].encoded;
        int32_t word_value = vpc->storage[i].value; /* Copy word value */
        label_address = 0;                          /* Reset label address */
        value = 0;                                  /* Reset computed value */

        /* Check if the encoded string starts with '&' (Relative Addressing) */
        if (encoded_str[0] == '&')
        {
            /* Remove '&' and find the corresponding label */
            for (j = 0; j < label_table->count; j++)
            {
                if (strcmp(&encoded_str[1], label_table->labels[j].name) == 0)
                {
                    label_address = label_table->labels[j].address;
                    value = label_address - (i - 1); /* Compute relative value */

                    /* Modify word value: Insert `value` into bits 3-23 */
                    word_value &= ~(0x1FFFFF << 3);        /* Clear bits 3-23 */
                    word_value |= (value & 0x1FFFFF) << 3; /* Set bits 3-23 */
                    vpc->storage[i].value = word_value;    /* Store modified value */

                    break;
                }
            }

            printf("| %-7u | %-20s | 0x%06X | %-22d |\n", i, encoded_str, word_value, value);
            continue;
        }

        /* Check if the encoded string matches any label in the label table */
        for (j = 0; j < label_table->count; j++)
        {
            if (strcmp(encoded_str, label_table->labels[j].name) == 0)
            {
                label_address = label_table->labels[j].address;
                value = label_address; /* Direct label address */

                /* Reset word value */
                vpc->storage[i].value = 0;

                /* Insert `value` into bits 3-23 */
                vpc->storage[i].value |= (value & 0x1FFFFF) << 3; /* Store value in bits 3-23 */

                /* Ensure bit 2 is 0 */
                vpc->storage[i].value &= ~(1 << 2);

                /* Set bits 0 and 1 based on label type */
                if (strcmp(label_table->labels[j].type, "external") == 0)
                {
                    vpc->storage[i].value |= (1 << 0);  /* Bit 0 = 1 */
                    vpc->storage[i].value &= ~(1 << 1); /* Bit 1 = 0 */
                }
                else
                {
                    vpc->storage[i].value &= ~(1 << 0); /* Bit 0 = 0 */
                    vpc->storage[i].value |= (1 << 1);  /* Bit 1 = 1 */
                }

                printf("| %-7u | %-20s | 0x%06X | %-22d |\n", i, encoded_str, vpc->storage[i].value, value);
                break;
            }
        }
    }

    printf("--------------------------------------------------------------------------\n");
}
