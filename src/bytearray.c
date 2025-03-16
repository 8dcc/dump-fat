
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/bytearray.h"
#include "include/util.h"

/*
 * TODO: Perhaps change 'fill_column' argument to 'words_per_line'.
 */
void bytearray_print_at(FILE* fp,
                        const ByteArray* arr,
                        size_t visual_offset,
                        size_t word_size,
                        size_t fill_column) {
    const bool split_words = (word_size >= 1);
    if (!split_words)
        word_size = 1;

    const size_t total_word_chars =
      word_size * STRLEN("FF") + ((split_words) ? STRLEN(" ") : 0);
    const size_t words_per_line =
      (fill_column - STRLEN("0000: ")) / total_word_chars;

    /* First offset */
    fprintf(fp, "%04zX: ", visual_offset);

    for (size_t i = 0; i < arr->size; i++) {
        /* Current byte */
        fprintf(fp, "%02X", ((uint8_t*)arr->data)[i]);

        const size_t bytes_printed = (i + 1);
        if (bytes_printed % word_size == 0) {
            /* Just ended a word */
            const size_t words_printed = bytes_printed / word_size;
            if (words_per_line != 0 && words_printed % words_per_line == 0)
                fprintf(fp, "\n%04zX: ", visual_offset + i + 1);
            else if (split_words)
                fputc(' ', fp);
        }
    }

    /* TODO: Print final newline when needed. */
}
