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
 *
 * ----------------------------------------------------------------------------
 *
 * Note that all references to the FAT specification refer to version 1.03 (i.e.
 * 'fatgen103.pdf'). See also my blog article on the FAT file system:
 * https://8dcc.github.io/programming/understanding-fat.html
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h> /* FILE */
#include <stdlib.h>
#include <string.h>

#include "include/bytearray.h"
#include "include/fat12.h"

/*----------------------------------------------------------------------------*/
/* General disk reading */

/*
 * Append 'count' sectors at the specified 'lba' address from the specified
 * 'disk' into the specified 'dst' byte array. The 'dst->data' pointer may be
 * NULL, and a new buffer will be allocated.
 *
 * On success, this function returns true and the caller is responsible for
 * freeing the new value in 'dst->data'. On failure, this function returns false
 * and the 'dst' array will remain unchanged, with the old data.
 */
static bool append_sectors(ByteArray* dst,
                           FILE* disk,
                           const ExtendedBPB* ebpb,
                           uint16_t lba,
                           uint8_t count) {
    if (fseek(disk, lba * ebpb->bytes_per_sector, SEEK_SET) != 0)
        return false;

    /* Allocate a new data buffer, without modifying the one on 'dst' */
    size_t new_size = dst->size + (count * ebpb->bytes_per_sector);
    void* new_data  = malloc(new_size);
    if (new_data == NULL)
        return false;

    /* Copy the old data to the new buffer */
    if (dst->data != NULL)
        memcpy(new_data, dst->data, dst->size);

    /* Read the new data into the free space of the new data buffer */
    void* free_data = (char*)new_data + dst->size;
    if (fread(free_data, ebpb->bytes_per_sector, count, disk) != count) {
        free(new_data);
        return false;
    }

    /*
     * Once every operation succeeded, we can modify 'dst'. First, free the old
     * 'data' pointer, which we are "reallocating". Then, overwrite the data
     * pointer and size with the updated values.
     */
    free(dst->data);
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

/*
 * Return the LBA address after the end of the root directory.
 *
 * Note that the root directory might only use a fraction of its last sector; we
 * will return the LBA address of the next one. This is consistent with the
 * behavior of 'get_rootdir_size', which includes the last (partial) one.
 */
static inline size_t get_data_region_start(const ExtendedBPB* ebpb) {
    return get_rootdir_start(ebpb) + get_rootdir_size(ebpb);
}

DirectoryEntry* read_root_directory(FILE* disk, const ExtendedBPB* ebpb) {
    const size_t lba_start    = get_rootdir_start(ebpb);
    const size_t size_sectors = get_rootdir_size(ebpb);

    ByteArray result;
    if (!read_sectors(&result, disk, ebpb, lba_start, size_sectors))
        return NULL;

    return result.data;
}

/*----------------------------------------------------------------------------*/
/* Files */

/*
 * Return the cluster number that is linked to the specified cluster in a 12-bit
 * File Allocation Table.
 */
static inline uint16_t fat12_get_linked_cluster(ByteArray fat,
                                                uint16_t cluster) {
    /*
     * Since each entry in the FAT is 12 bits (i.e. 3 nibbles, one byte and
     * half), we need to multiply it by 2/3. We can easily do this by adding
     * half of the cluster, as shown in p. 16 of the specification. Note that
     * this rounds down.
     */
    const int fat_idx = cluster + (cluster / 2);

    const uint8_t byte0 = ((uint8_t*)fat.data)[fat_idx];
    const uint8_t byte1 = ((uint8_t*)fat.data)[fat_idx + 1];
    if (cluster & 1) {
        /*
         * Odd cluster number. In a big-endian machine, the FAT would look
         * something like this:
         *
         *     AA AB BB CC CD DD
         *        ^
         *
         * But since FAT12 is little-endian, the nibble order of 0x123 in entry
         * 1 would be:
         *
         *     ?? 3? 12 ?? ?? ??
         *        ^
         */
        return (uint16_t)byte1 << 4 | byte0 >> 4;
    } else {
        /*
         * Even cluster number. The FAT would look something like this:
         *
         *     AA AB BB CC CD DD
         *              ^
         *
         * But since FAT12 is little-endian, the nibble order of 0x123 in entry
         * 2 would be:
         *
         *     ?? ?? ?? 23 ?1 ??
         *              ^
         */
        return (uint16_t)(byte1 & 0x0F) << 8 | byte0;
    }
}

DirectoryEntry* search_entry(DirectoryEntry* arr,
                             size_t size,
                             const char* name) {
    for (size_t i = 0; i < size; i++)
        if (memcmp(name, arr[i].name, sizeof(arr[i].name)) == 0)
            return &arr[i];
    return NULL;
}

bool read_file(ByteArray* dst,
               FILE* disk,
               const ExtendedBPB* ebpb,
               ByteArray fat,
               const DirectoryEntry* file) {
    dst->data = NULL;
    dst->size = 0;

    const size_t data_region_start = get_data_region_start(ebpb);

    /*
     * The first cluster where the file is stored. Clusters are simply groups
     * contiguous of sectors, and their size is determined by
     * 'ExtendedBPB.sectors_per_cluster'. Note that FAT12 and FAT16 ignore
     * 'DirectoryEntry.first_cluster_high'.
     *
     * This actually indicates the index in the FAT where the linked list of
     * clusters start. The "real" cluster number in the volume will be calulated
     * below.
     */
    uint16_t current_cluster = file->first_cluster_low;

    while (current_cluster < 0xFF8) {
        /*
         * As explained above, the cluster number that we have is used to
         * calculate the index in the FAT, not the sector number in the disk.
         * Since the first two entries of the FAT are reserved, the first data
         * cluster is the third one, so we subtract 2 from it before multiplying
         * it by the 'sectors_per_cluster' field. See p. 14 of the
         * specification.
         */
        const uint32_t sector_lba =
          data_region_start +
          ((current_cluster - 2) * ebpb->sectors_per_cluster);

        if (!append_sectors(dst,
                            disk,
                            ebpb,
                            sector_lba,
                            ebpb->sectors_per_cluster)) {
            free(dst->data);
            return false;
        }

        /*
         * Move to the next cluster in the FAT linked list.
         */
        current_cluster = fat12_get_linked_cluster(fat, current_cluster);
    }

    /*
     * TODO: Perhaps set 'dst->size' to the actual size of the file after
     * reading.
     */
    return true;
}
