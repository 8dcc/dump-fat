
#include <stdbool.h>
#include <stddef.h>

#include "include/util.h"

bool is_mem_zero(const void* ptr, size_t size) {
    const char* arr = ptr;
    while (size-- > 0)
        if (*arr++ != 0)
            return false;
    return true;
}
