
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/bytearray.h"
#include "include/util.h"

/*
 * Word size in bytes. A space will be printed between words. Set to zero to
 * disable.
 */
#define WORD_SIZE 2

/*
 * Maximum column used for printing the arrays. Set to zero to disable.
 */
#define FILL_COLUMN 80

void bytearray_print(FILE* fp, ByteArray* arr) {
    const bool split_words = (WORD_SIZE >= 1);
    const size_t word_size = (split_words) ? WORD_SIZE : 1;

    const size_t total_word_chars =
      word_size * STRLEN("FF") + ((split_words) ? STRLEN(" ") : 0);
    const size_t words_per_line = FILL_COLUMN / total_word_chars;

    for (size_t i = 0; i < arr->size; i++) {
        fprintf(fp, "%02X", ((uint8_t*)arr->data)[i]);

        const size_t bytes_printed = (i + 1);
        if (bytes_printed % word_size == 0) { /* Just ended a word */
            const size_t words_printed = bytes_printed / word_size;
            if (words_per_line != 0 && words_printed % words_per_line == 0)
                fputc('\n', fp);
            else if (split_words)
                fputc(' ', fp);
        }
    }
}
