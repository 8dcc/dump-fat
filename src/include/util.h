
#ifndef UTIL_H_
#define UTIL_H_ 1

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Assert condition at compile-time.
 */
#define STATIC_ASSERT(COND) _Static_assert(COND, "Assertion failed.")

/*
 * Return the length of a string literal without the NULL terminator.
 */
#define STRLEN(STR) (sizeof(STR) - sizeof(char))

/*
 * Return the number of elements in an array.
 */
#define ARRLEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

/*
 * Print a formatted string to 'stderr' along with a newline.
 */
#define ERR(...)                                                               \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        fputc('\n', stderr);                                                   \
    } while (0)

/*----------------------------------------------------------------------------*/

/*
 * Return true if the bytes in the specified memory region are all zero.
 */
bool is_mem_zero(const void* ptr, size_t size);

#endif /* UTIL_H_ */
