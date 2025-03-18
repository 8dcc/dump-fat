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
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h> /* FILE */
#include <stdlib.h>

#include "include/bytearray.h"
#include "include/fat12.h"

/*----------------------------------------------------------------------------*/
/* General disk reading */

static bool append_sectors(ByteArray* dst,
                           FILE* disk,
                           const ExtendedBPB* ebpb,
                           uint16_t lba,
                           uint8_t count) {
    if (fseek(disk, lba * ebpb->bytes_per_sector, SEEK_SET) != 0)
        return false;

    size_t new_size = dst->size + (count * ebpb->bytes_per_sector);
    void* new_data  = realloc(dst->data, new_size);
    if (new_data == NULL)
        return false;

    void* free_data = (char*)new_data + dst->size;
    if (fread(free_data, ebpb->bytes_per_sector, count, disk) != count) {
        if (dst->data == NULL)
            free(new_data);
        return false;
    }

    dst->data = new_data;
    dst->size = new_size;
    return true;
}

BootSector* read_boot_sector(FILE* disk) {
    BootSector* result = malloc(sizeof(BootSector));
    if (result == NULL)
        return NULL;

    if (fread(result, sizeof(BootSector), 1, disk) <= 0) {
        free(result);
        return NULL;
    }

    return result;
}

bool read_sectors(ByteArray* dst,
                  FILE* disk,
                  const ExtendedBPB* ebpb,
                  uint16_t lba,
                  uint8_t count) {
    dst->data = NULL;
    dst->size = 0;
    return append_sectors(dst, disk, ebpb, lba, count);
}

/*----------------------------------------------------------------------------*/
/* File Allocation Table (FAT) */

bool read_fat(ByteArray* dst, FILE* disk, const ExtendedBPB* ebpb) {
    /* The FAT region starts right after the reserved sectors */
    return read_sectors(dst,
                        disk,
                        ebpb,
                        ebpb->reserved_sectors,
                        ebpb->sectors_per_fat);
}

/*----------------------------------------------------------------------------*/
/* Root directory */

/*
 * Return the LBA address where the root directory starts.
 *
 * The root directory region starts after the reserved sectors and after the
 * FAT(s).
 */
static inline size_t get_rootdir_start(const ExtendedBPB* ebpb) {
    return ebpb->reserved_sectors + ebpb->sectors_per_fat * ebpb->fat_count;
}

/*
 * Return the size of the root directory in sectors.
 */
static inline size_t get_rootdir_size(const ExtendedBPB* ebpb) {
    /*
     * Size of the root directory in bytes.
     */
    const size_t size_bytes = ebpb->dir_entries_count * sizeof(DirectoryEntry);

    /*
     * Size of the root directory in sectors. We add (BytesPerSector-1) to the
     * size in bytes to round up the sector count, in case the root directory
     * only uses a fraction of its last sector (i.e. the division wasn't exact).
     *
     * This is the same operation that is used in p. 13 of the specification.
     */
    return (size_bytes + ebpb->bytes_per_sector - 1) / ebpb->bytes_per_sector;
}

DirectoryEntry* read_root_directory(FILE* disk, const ExtendedBPB* ebpb) {
    const size_t lba_start    = get_rootdir_start(ebpb);
    const size_t size_sectors = get_rootdir_size(ebpb);

    ByteArray result;
    if (!read_sectors(&result, disk, ebpb, lba_start, size_sectors))
        return NULL;

    return result.data;
}
