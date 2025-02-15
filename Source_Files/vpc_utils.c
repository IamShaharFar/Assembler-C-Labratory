/* vpc_utils.c */
#include "../Header_Files/vpc_utils.h"
#include "../Header_Files/errors.h"

int add_word_to_vpc(VirtualPC *vpc, Word word) {
    if (vpc->IC >= STORAGE_SIZE) {
        print_error_no_line(ERROR_MEMORY_ALLOCATION);
        return 0;
    }

    vpc->storage[vpc->IC] = word;
    vpc->IC++;
    return 1;
}
