/*
 * Copyright 2025 8dcc
 *
 * This file is part of dump-fat.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
 * The 'words_per_line' argument indicates the maximum number of words (not
 * bytes) that can fit in single line before switching to the next one. Set to
 * zero to disable line wrapping.
 */
void bytearray_print_at(FILE* fp,
                        const ByteArray* arr,
                        size_t visual_offset,
                        size_t word_size,
                        size_t words_per_line);

/*
 * Simple wrapper for 'bytearray_print_at'.
 */
static inline void bytearray_print(FILE* fp, ByteArray* arr) {
    const size_t default_word_size      = 2;
    const size_t default_words_per_line = 0x10 / default_word_size;
    bytearray_print_at(fp, arr, 0, default_word_size, default_words_per_line);
}

#endif /* BYTE_ARRAY_H_ */
