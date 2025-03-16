
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
                  BootSector* boot_sector,
                  uint16_t lba,
                  uint8_t count) {
    if (fseek(disk, lba * boot_sector->ebpb.bytes_per_sector, SEEK_SET) != 0)
        return false;

    size_t result_sz = boot_sector->ebpb.bytes_per_sector * count;
    void* result     = malloc(result_sz);
    if (result == NULL)
        return false;

    if (fread(result, boot_sector->ebpb.bytes_per_sector, count, disk) !=
        count) {
        free(result);
        return false;
    }

    dst->data = result;
    dst->size = result_sz;
    return true;
}

bool read_fat(ByteArray* dst, FILE* disk, BootSector* boot_sector) {
    /* The FAT region starts right after the reserved sectors */
    return read_sectors(dst,
                        disk,
                        boot_sector,
                        boot_sector->ebpb.reserved_sectors,
                        boot_sector->ebpb.sectors_per_fat);
}
