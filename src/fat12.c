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
    if (fseek(disk, lba * ebpb->bytes_per_sector, SEEK_SET) != 0)
        return false;

    size_t result_sz = ebpb->bytes_per_sector * count;
    void* result     = malloc(result_sz);
    if (result == NULL)
        return false;

    if (fread(result, ebpb->bytes_per_sector, count, disk) != count) {
        free(result);
        return false;
    }

    dst->data = result;
    dst->size = result_sz;
    return true;
}

bool read_fat(ByteArray* dst, FILE* disk, const ExtendedBPB* ebpb) {
    /* The FAT region starts right after the reserved sectors */
    return read_sectors(dst,
                        disk,
                        ebpb,
                        ebpb->reserved_sectors,
                        ebpb->sectors_per_fat);
}

DirectoryEntry* read_root_directory(FILE* disk, const ExtendedBPB* ebpb) {
    /*
     * The root directory region starts after the reserved sectors and after the
     * FAT(s).
     */
    const size_t lba_start =
      ebpb->reserved_sectors + ebpb->sectors_per_fat * ebpb->fat_count;

    /*
     * Size of the root directory in bytes and sectors. We add
     * (BytesPerSector-1) to the size in bytes to round up the sector count, in
     * case the root directory only uses a fraction of its last sector (i.e. the
     * division wasn't exact).
     */
    const size_t size_bytes = ebpb->dir_entries_count * sizeof(DirectoryEntry);
    const size_t size_sectors =
      (size_bytes + ebpb->bytes_per_sector - 1) / ebpb->bytes_per_sector;

    ByteArray result;
    if (!read_sectors(&result, disk, ebpb, lba_start, size_sectors))
        return NULL;

    return result.data;
}
