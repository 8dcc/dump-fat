
#ifndef BYTE_ARRAY_H_
#define BYTE_ARRAY_H_ 1

#include <stddef.h>
#include <stdio.h>  /* FILE */

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
 */
void bytearray_print(FILE* fp, ByteArray* arr);

#endif /* BYTE_ARRAY_H_ */
