
#ifndef FAT12_H_
#define FAT12_H_ 1

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h> /* FILE */

#include "util.h" /* STATIC_ASSERT */
#include "bytearray.h"

/*
 * Extended BIOS Parameter Block (EBPB) used by FAT12 and FAT16 since DOS 4.0.
 *
 * See:
 * https://en.wikipedia.org/wiki/DOS_4.0_EBPB
 */
typedef struct {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t dir_entries_count;
    uint16_t total_sectors;
    uint8_t media_descriptor_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t large_sector_count;

    /* Extended */
    uint8_t drive_number;
    uint8_t reserved;
    uint8_t signature;
    uint8_t volume_id[4];
    uint8_t volume_label[11];
    uint8_t system_id[8];
} __attribute__((packed)) ExtendedBPB;
STATIC_ASSERT(sizeof(ExtendedBPB) == 51);

/*
 * First boot sector of a FAT12 disk.
 */
typedef struct {
    uint16_t short_jmp;
    uint8_t nop;
    uint8_t oem_identifier[8];
    ExtendedBPB ebpb;

    /*
     * After the EBPB, there is code until offset 510 included, followed by byte
     * 0x55 and byte 0xAA.
     */
} __attribute__((packed)) BootSector;

/*
 * Individual entry in a FAT12 directory.
 *
 * The filename limit was 11 characters, with spaces separating the name itself
 * from the extension (e.g.  "FOOBAR TXT").
 *
 * See:
 * https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#Directory_entry
 */
typedef struct {
    char name[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t created_time_tenths;
    uint16_t created_time;
    uint16_t created_date;
    uint16_t accessed_date;
    uint16_t first_cluster_high; /* Unused in FAT12 and FAT16 */
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t first_cluster_low;
    uint32_t size;
} __attribute__((packed)) DirectoryEntry;
STATIC_ASSERT(sizeof(DirectoryEntry) == 32);

/*----------------------------------------------------------------------------*/

/*
 * TODO: Add more consistent naming convention (e.g. 'fat12_*').
 */

/*
 * Return a heap-allocated copy of the boot sector in the specified file. The
 * returned pointer must be freed by the caller.
 */
BootSector* read_boot_sector(FILE* disk);

/*
 * Read the specified number of sectors from the specified Logical Block Address
 * (LBA) of the specified disk image file. The returned pointer must be freed by
 * the caller.
 *
 * Note that this function will overwrite the current position in the disk file,
 * so the caller might want to use something like 'ftell' before the call.
 */
bool read_sectors(ByteArray* dst,
                  FILE* disk,
                  const ExtendedBPB* ebpb,
                  uint16_t lba,
                  uint8_t count);

/*
 * Read the File Allocation Table (FAT) of the specified disk image file.
 *
 * The 'data' pointer of the received 'ByteArray' structure will be set to a
 * heap-allocated pointer that the caller must free.
 */
bool read_fat(ByteArray* dst, FILE* disk, const ExtendedBPB* ebpb);

/*
 * Return an array of directory entries for the root directory of the specified
 * disk. The size of the returned array can be obtained by looking at the
 * 'dir_entries_count' member of the 'ExtendedBPB' structure.
 *
 * The caller is responsible for freeing the returned pointer.
 */
DirectoryEntry* read_root_directory(FILE* disk, const ExtendedBPB* ebpb);

#endif /* FAT12_H_ */
