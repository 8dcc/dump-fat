
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/bytearray.h"
#include "include/util.h"

void bytearray_print_at(FILE* fp,
                        const ByteArray* arr,
                        size_t visual_offset,
                        size_t word_size,
                        size_t words_per_line) {
    const bool split_words = (word_size >= 1);
    if (!split_words)
        word_size = 1;

    /* First offset */
    fprintf(fp, "%04zX: ", visual_offset);

    for (size_t i = 0; i < arr->size; i++) {
        /* Current byte */
        fprintf(fp, "%02X", ((uint8_t*)arr->data)[i]);

        const size_t bytes_printed = (i + 1);
        if (i + 1 < arr->size && bytes_printed % word_size == 0) {
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
