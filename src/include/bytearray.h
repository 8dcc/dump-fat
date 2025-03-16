
#ifndef BYTE_ARRAY_H_
#define BYTE_ARRAY_H_ 1

#include <stddef.h>
#include <stdio.h> /* FILE */

/*
 * Generic array structure.
 */
typedef struct {
    void* data;
    size_t size;
} ByteArray;

/*----------------------------------------------------------------------------*/

/*
 * Print the specified byte array to the specified file.
 *
 * The 'visual_offset' argument indicates the initial offset to be displayed
 * when printing the file, but not the actual offset when reading the array.
 *
 * The 'word_size' argument indicates the number of bytes (not bits) to be
 * grouped together by spaces. Set to zero to disable word grouping.
 *
 * The 'fill_column' argument indicates the maximum number of characters that a
 * single line can use before switching to the next one. Set to zero to disable
 * line wrapping.
 */
void bytearray_print_at(FILE* fp,
                        const ByteArray* arr,
                        size_t visual_offset,
                        size_t word_size,
                        size_t fill_column);

/*
 * Simple wrapper for 'bytearray_print_at'.
 */
static inline void bytearray_print(FILE* fp, ByteArray* arr) {
    const size_t default_word_size   = 2;
    const size_t default_fill_column = 80;
    bytearray_print_at(fp, arr, 0, default_word_size, default_fill_column);
}

#endif /* BYTE_ARRAY_H_ */
