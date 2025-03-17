
#ifndef PRINT_H_
#define PRINT_H_ 1

#include <stdio.h> /* FILE */

#include "fat12.h"

/*
 * Print the data in the specified Extended Bios Parameter Block (EBPB) to the
 * specified file.
 */
void print_ebpb(FILE* fp, const ExtendedBPB* ebpb);

/*
 * Print an array of directory entries of the specified size to the specified
 * file.
 */
void print_directory_entries(FILE* fp, const DirectoryEntry* arr, size_t size);

#endif /* PRINT_H_ */
