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

#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/bytearray.h"
#include "include/fat12.h"
#include "include/print.h"

int main(int argc, char** argv) {
    int exit_code = 0;

    if (argc < 2 || argc > 3) {
        ERR("Usage: %s DISK.img [FILENAME]\n", argv[0]);
        return 1;
    }

    const char* diskimg_path = argv[1];
    FILE* diskimg_fp         = fopen(diskimg_path, "rb");
    if (diskimg_fp == NULL) {
        ERR("Error opening '%s': %s", diskimg_path, strerror(errno));
        return 1;
    }

    BootSector* boot_sector = read_boot_sector(diskimg_fp);
    if (boot_sector == NULL) {
        ERR("Could not read boot sector of '%s'.", diskimg_path);
        exit_code = 1;
        goto invalid_boot_sector;
    }

    puts("Extended Bios Parameter Block (EBPB):");
    print_ebpb(stdout, &boot_sector->ebpb);

    ByteArray fat;
    if (!read_fat(&fat, diskimg_fp, &boot_sector->ebpb)) {
        ERR("Could not read FAT of '%s'.", diskimg_path);
        exit_code = 1;
        goto invalid_fat;
    }

    putchar('\n');
    puts("File Allocation Table (FAT):");
    bytearray_print(stdout, fat);

    DirectoryEntry* root_directory =
      read_root_directory(diskimg_fp, &boot_sector->ebpb);
    if (root_directory == NULL) {
        ERR("Could not read root directory of '%s'.", diskimg_path);
        exit_code = 1;
        goto invalid_root_directory;
    }

    putchar('\n');
    puts("Root directory:");
    print_directory_entries(stdout,
                            root_directory,
                            boot_sector->ebpb.dir_entries_count);

    if (argc >= 3) {
        const char* filename = argv[2];
        if (strlen(filename) != 11) {
            ERR("Invalid filename, expected 11 characters, got '%s'.",
                filename);
            exit_code = 1;
            goto invalid_file;
        }

        DirectoryEntry* file = search_entry(root_directory,
                                            boot_sector->ebpb.dir_entries_count,
                                            filename);
        if (file == NULL) {
            ERR("File '%s' is not present in the root directory.", filename);
            exit_code = 1;
            goto invalid_file;
        }

        ByteArray file_contents;
        if (read_file(&file_contents,
                      diskimg_fp,
                      &boot_sector->ebpb,
                      fat,
                      file)) {
            putchar('\n');
            printf("Contents of '%s':\n", filename);
            bytearray_print(stdout, file_contents);
            free(file_contents.data);
        } else {
            ERR("Could not read root directory of '%s'.", diskimg_path);
        }
    }

invalid_file:
    free(root_directory);
invalid_root_directory:
    free(fat.data);
invalid_fat:
    free(boot_sector);
invalid_boot_sector:
    fclose(diskimg_fp);

    return exit_code;
}
