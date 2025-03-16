
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

#endif /* UTIL_H_ */
