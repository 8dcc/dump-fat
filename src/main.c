
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/bytearray.h"
#include "include/fat12.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s DISK.img\n", argv[0]);
        return 1;
    }

    const char* diskimg_path = argv[1];
    FILE* diskimg_fp         = fopen(diskimg_path, "rb");
    if (diskimg_fp == NULL) {
        fprintf(stderr,
                "Error opening '%s': %s",
                diskimg_path,
                strerror(errno));
        return 1;
    }

    BootSector* boot_sector = read_boot_sector(diskimg_fp);
    if (boot_sector == NULL) {
        fprintf(stderr, "Could not read boot sector of '%s'.", diskimg_path);
        fclose(diskimg_fp);
        return 1;
    }

    ByteArray fat;
    if (!read_fat(&fat, diskimg_fp, boot_sector)) {
        fprintf(stderr, "Could not read FAT of '%s'.", diskimg_path);
        free(boot_sector);
        fclose(diskimg_fp);
        return 1;
    }

    bytearray_print(stdout, &fat);

    free(fat.data);
    free(boot_sector);
    fclose(diskimg_fp);
    return 0;
}
