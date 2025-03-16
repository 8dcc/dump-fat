
#ifndef PRINT_H_
#define PRINT_H_ 1

#include <stdio.h> /* FILE */

#include "fat12.h"

/*
 * Print the data in the specified Extended Bios Parameter Block (EBPB) to the
 * specified file.
 */
void print_ebpb(FILE* fp, const ExtendedBiosParameterBlock* ebpb);

#endif /* PRINT_H_ */
