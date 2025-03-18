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
