/* Source_Files/vpc_utils.c */
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

/* Processes a .data or .string directive and stores the values in the VirtualPC storage. */
int process_data_or_string_directive(char *ptr, VirtualPC *vpc, int *storage_full)
{
    int count = 0; /* keeps track of stored elements */
    ptr = advance_to_next_token(ptr);

    if (strncmp(ptr, ".data", 5) == 0) /* check if the directive is .data */
    {
        ptr += 5;
        ptr = advance_to_next_token(ptr);

        /* iterate over numbers */
        while (*ptr)
        {
            char *endptr;
            int num = strtol(ptr, &endptr, 10); /* convert number */

            /* check if the number is valid */
            if (ptr == endptr)
                break;

            if (vpc->DC + vpc->IC < STORAGE_SIZE)
            {
                vpc->storage[vpc->last_adress].value = num & 0xFFFFFF; /* store 24-bit value */
                sprintf(vpc->storage[vpc->last_adress].encoded, "%d", num); /* store as string */
                vpc->storage[vpc->last_adress].encoded[sizeof(vpc->storage[vpc-> last_adress].encoded) - 1] = '\0'; /* ensure null-termination */
                vpc->last_adress++;
            }
            else
            {
                *storage_full = TRUE;
            }

            count++;
            ptr = endptr; /* move to next token */
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

        /* check for valid string */
        if (*ptr == '"')
        {
            ptr++;

            /* iterate over characters in the string */
            while (*ptr && *ptr != '"')
            {
                if (vpc->DC + vpc->IC < STORAGE_SIZE)
                {
                    vpc->storage[vpc->last_adress].value = (int)(*ptr) & 0xFFFFFF; /* store 24-bit character */
                    sprintf(vpc->storage[vpc->last_adress].encoded, "%c", *ptr);   /* store as string */
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
                vpc->storage[vpc->last_adress].value = 0; /* store null terminator */
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

/* Generates words from a command from a valid line of command and stores it in the VirtualPC storage. */
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
    int param_flags[2] = {0, 0}; /* flags for parameter words */
    int opcode = -1, funct = -1;

    /* create modifiable copy of input line */
    modifiable_line = malloc(strlen(line) + 1);
    if (!modifiable_line)
    {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return 0;
    }
    strcpy(modifiable_line, line);

    /* extract command name */
    ptr = advance_to_next_token(modifiable_line);
    sscanf(ptr, "%s", command);
    ptr = advance_past_token(ptr);
    ptr = advance_to_next_token(ptr);

    /* find expected parameter count */
    expected_params = get_expected_params(command);

    /* extract parameters based on expected count */
    if (expected_params == 1)
    {
        sscanf(ptr, "%s", param1);
    }
    else if (expected_params == 2)
    {
        sscanf(ptr, "%[^, ]", param1); /* read first parameter */
        ptr = advance_past_token_or_comma(ptr);
        ptr = advance_to_next_token(ptr);
        ptr++;
        sscanf(ptr, "%s", param2); /* read second parameter */
    }

    /* find opcode and funct code */
    for (i = 0; i < RESERVED_COMMANDS_COUNT; i++)
    {
        if (strcmp(commands_info[i].name, command) == 0)
        {
            opcode = commands_info[i].opcode;
            funct = commands_info[i].funct;
            break;
        }
    }

    /* check if command is valid */
    if (opcode == -1)
    {
        print_error_no_line(ERROR_UNKNOWN_COMMAND);
        free(modifiable_line);
        return 0;
    }

    /* process parameters and add words to vpc if needed */
    if (expected_params == 1)
    {
        process_operand(param1, &first_word, &second_word, 11, 8, &param_flags[0]);
    }
    else if (expected_params == 2)
    {
        process_operand(param1, &first_word, &second_word, 16, 13, &param_flags[0]);
        process_operand(param2, &first_word, &third_word, 11, 8, &param_flags[1]);
    }

    /* set bit 2 to 1 */
    first_word |= (1 << 2);

    /* set bits 18-23 for opcode */
    first_word |= (opcode & 0x3F) << 18; /* mask with 0x3F (111111) and shift left by 18 bits */

    /* set bits 3-7 for funct */
    if (funct != -1)
    {
        /* set bits 3-7 for funct */
        first_word |= (funct & 0x1F) << 3; /* mask with 0x1F (11111) and shift left by 3 bits */
    }

    /* store command word in vpc */
    if (vpc->IC < STORAGE_SIZE)
    {
        vpc->storage[vpc->IC].value = first_word;
        strncpy(vpc->storage[vpc->IC].encoded, command, sizeof(vpc->storage[vpc->IC].encoded) - 1); 
        vpc->storage[vpc->IC].encoded[sizeof(vpc->storage[vpc->IC].encoded) - 1] = '\0';
        vpc->IC++;
        vpc->last_adress++;
    }
    else
    {
        *storage_full = TRUE;
    }

    /* store additional parameter words if needed */
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
    return 1 + param_flags[0] + param_flags[1]; /* return number of stored words */
}

/* Processes an operand and updates the binary representation in the provided words. */
void process_operand(const char *param, unsigned int *first_word, unsigned int *word, int shift_opcode, int shift_reg, int *param_flag)
{
    int value;

    /* check if operand is not a register */
    if (!validate_register_operand(param))
    {
        *param_flag = 1; /* mark that an additional word is needed */

        if (param[0] == '#') /* immediate addressing */
        {
            *first_word |= (0 & 0x03) << shift_opcode; /* set addressing mode */
            *first_word |= (0 & 0x07) << shift_reg;  /* no register */
            value = atoi(&param[1]); /* extract number */

            /* handle negative values using two's complement */
            if (value < 0)
            {
                value = (1 << 21) + value;
            }

            *word |= (value & 0x1FFFFF) << 3; /* mask the value to 21 bits (0x1FFFFF, which is 21 bits set to 1) and store it in bits 3-23 of the word */
            *word |= (1 << 2);                /* Set bit 2 to 1 */
        }
        else if (param[0] == '&') /* relative addressing */
        {
            *first_word |= (2 & 0x03) << shift_opcode; 
            *first_word |= (0 & 0x07) << shift_reg;  
            *word |= (1 << 2); /* set bit 2 to 1 */
        }
        else /* direct addressing */
        {
            *first_word |= (1 & 0x03) << shift_opcode; 
            *first_word |= (0 & 0x07) << shift_reg;
            *word = 0; /* clear the word */
            *word |= 0x03; /* set bits 0-1 to 1 (11 in binary) in the second pass we will update the address and the R/E*/
        }
    }
    else /* register addressing */
    {
        int reg_num = atoi(&param[1]); /* extract register number */
        *first_word |= (reg_num & 0x07) << shift_reg; /* set the register bits with the register number */
        *first_word |= (3 & 0x03) << shift_opcode; /* register addressing - 3 */
    }
}
