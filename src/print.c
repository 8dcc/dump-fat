
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

#include "include/util.h"
#include "include/fat12.h"

#define PRINT_MEMBER(FP, STRUCT_PTR, MAXLEN, FMT, MEMBER_NAME)                 \
    fprintf(FP,                                                                \
            "%*.*s: %" FMT "\n",                                               \
            MAXLEN,                                                            \
            MAXLEN,                                                            \
            #MEMBER_NAME,                                                      \
            (STRUCT_PTR)->MEMBER_NAME)

#define PRINT_ARR_MEMBER(FP, STRUCT_PTR, MAXLEN, ITEM_FMT, MEMBER_NAME)        \
    do {                                                                       \
        fprintf(FP, "%*.*s: ", MAXLEN, MAXLEN, #MEMBER_NAME);                  \
        for (size_t i = 0; i < ARRLEN((STRUCT_PTR)->MEMBER_NAME); i++)         \
            fprintf(FP, "%" ITEM_FMT, ((STRUCT_PTR)->MEMBER_NAME)[i]);         \
        fputc('\n', FP);                                                       \
    } while (0)

void print_ebpb(FILE* fp, const ExtendedBiosParameterBlock* ebpb) {
    PRINT_MEMBER(fp, ebpb, 21, PRId16, bytes_per_sector);
    PRINT_MEMBER(fp, ebpb, 21, PRId8, sectors_per_cluster);
    PRINT_MEMBER(fp, ebpb, 21, PRId16, reserved_sectors);
    PRINT_MEMBER(fp, ebpb, 21, PRId8, fat_count);
    PRINT_MEMBER(fp, ebpb, 21, PRId16, dir_entries_count);
    PRINT_MEMBER(fp, ebpb, 21, PRId16, total_sectors);
    PRINT_MEMBER(fp, ebpb, 21, PRId8, media_descriptor_type);
    PRINT_MEMBER(fp, ebpb, 21, PRId16, sectors_per_fat);
    PRINT_MEMBER(fp, ebpb, 21, PRId16, sectors_per_track);
    PRINT_MEMBER(fp, ebpb, 21, PRId16, heads);
    PRINT_MEMBER(fp, ebpb, 21, PRId32, hidden_sectors);
    PRINT_MEMBER(fp, ebpb, 21, PRId32, large_sector_count);

    /* Extended */
    fputc('\n', fp);
    PRINT_MEMBER(fp, ebpb, 21, PRId8, drive_number);
    PRINT_MEMBER(fp, ebpb, 21, PRId8, reserved);
    PRINT_MEMBER(fp, ebpb, 21, PRId8, signature);
    PRINT_ARR_MEMBER(fp, ebpb, 21, "02" PRIX8 " ", volume_id);
    PRINT_MEMBER(fp, ebpb, 21, ".11s", volume_label);
    PRINT_MEMBER(fp, ebpb, 21, ".8s", system_id);
}
