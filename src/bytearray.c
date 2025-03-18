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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/bytearray.h"
#include "include/util.h"

void bytearray_print_at(FILE* fp,
                        ByteArray arr,
                        size_t visual_offset,
                        size_t word_size,
                        size_t words_per_line) {
    const bool split_words = (word_size >= 1);
    if (!split_words)
        word_size = 1;

    /* First offset */
    fprintf(fp, "%04zX: ", visual_offset);

    for (size_t i = 0; i < arr.size; i++) {
        /* Current byte */
        fprintf(fp, "%02X", ((uint8_t*)arr.data)[i]);

        const size_t bytes_printed = (i + 1);
        if (i + 1 < arr.size && bytes_printed % word_size == 0) {
            /* Just ended a word */
            const size_t words_printed = bytes_printed / word_size;
            if (words_per_line != 0 && words_printed % words_per_line == 0)
                fprintf(fp, "\n%04zX: ", visual_offset + i + 1);
            else if (split_words)
                fputc(' ', fp);
        }
    }

    fputc('\n', fp);
}
