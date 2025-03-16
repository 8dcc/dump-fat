
#ifndef UTIL_H_
#define UTIL_H_ 1

/*
 * Assert condition at compile-time.
 */
#define STATIC_ASSERT(COND) _Static_assert(COND, "Assertion failed.")

/*
 * Return the length of a string literal without the NULL terminator.
 */
#define STRLEN(STR) (sizeof(STR) - sizeof(char))

/*
 * Print a formatted string to 'stderr' along with a newline.
 */
#define ERR(...)                                                               \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        fputc('\n', stderr);                                                   \
    } while (0)

#endif /* UTIL_H_ */
