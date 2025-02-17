/* vpc_utils.c */
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/errors.h"
#include "../Header_Files/utils.h"
#include "../Header_Files/globals.h"
#include "../Header_Files/first_pass_utils.h"
#include <stdio.h>
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
 * @brief Prints the binary representation of each element in a valid .data or .string directive.
 *
 * @param ptr Pointer to the content after .data or .string.
 */
void print_data_or_string_binary(char *ptr)
{
    char binary_str[25];
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
            printf("%s\n", binary_str);

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
                printf("%s\n", binary_str);
                ptr++;
            }
        }
    }
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

void generate_binary_command(const char *line)
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
    int opcode = -1, funct = -1;

    /* Make a modifiable copy of the input line */
    modifiable_line = malloc(strlen(line) + 1);
    if (!modifiable_line)
    {
        printf("Error: Memory allocation failed!\n");
        return;
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
        return;
    }

    if (expected_params == 1)
    {
        if (!validate_register_operand(param1))
        {
            if (param1[0] == '#')
            {
                first_word |= (0 & 0x03) << 11;         /* Set bits 11-12 to 0 */
                first_word |= (0 & 0x07) << 8;          /* Set bits 8-10 to 0 */

                int value = atoi(&param1[1]);           /* Convert number after '#' */
                second_word |= (value & 0x1FFFFF) << 3; /* Store value in bits 3-23 */
                second_word |= (1 << 2);                /* Set bit 2 to 1 */
            }
            else if (param1[0] == '&')
            {
                first_word |= (2 & 0x03) << 11; /* Set bits 11-12 to 2 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
            }
            else
            {
                first_word |= (1 & 0x03) << 11; /* Set bits 11-12 to 1 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
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
            if (param1[0] == '#')
            {
                first_word |= (0 & 0x03) << 16; /* Set bits 16-17 to 0 */
                first_word |= (0 & 0x07) << 13; /* Set bits 13-15 to 0 */
            }
            else if (param1[0] == '&')
            {
                first_word |= (2 & 0x03) << 16; /* Set bits 16-17 to 2 */
                first_word |= (0 & 0x07) << 13; /* Set bits 13-15 to 0 */
            }
            else
            {
                first_word |= (1 & 0x03) << 16; /* Set bits 16-17 to 1 */
                first_word |= (0 & 0x07) << 13; /* Set bits 13-15 to 0 */
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
            if (param2[0] == '#')
            {
                first_word |= (0 & 0x03) << 11; /* Set bits 11-12 to 0 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
            }
            else if (param2[0] == '&')
            {
                first_word |= (2 & 0x03) << 11; /* Set bits 11-12 to 2 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
            }
            else
            {
                first_word |= (1 & 0x03) << 11; /* Set bits 11-12 to 2 */
                first_word |= (0 & 0x07) << 8;  /* Set bits 8-10 to 0 */
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

    /* Print extracted values for debugging */
    printf("Command: %s\n", command);
    if (expected_params > 0)
        printf("Param 1: %s\n", param1);
    if (expected_params > 1)
        printf("Param 2: %s\n", param2);

    /* Print first word binary representation */
    printf("First binary word: ");
    print_binary(first_word, 24);
    printf("Second binary word: ");
    print_binary(second_word, 24);

    free(modifiable_line);
}