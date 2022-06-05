#include <lib/write_to_drive.h>
#include <string.h>
#include <types.h>
#define RESERVED_BLOCKS 16

// boot sector code in boot.asm
extern const uint8_t _binary_boot_bin_start[];

static struct drive_image* image;

static inline void wr_qword(uint64_t loc, uint64_t x) {
    fseek(image, (long)loc, SEEK_SET);
    fwrite(&x, 8, 1, image);
    return;
}

int main(int argc, char** argv) {
    const uint8_t* boot_sector = _binary_boot_bin_start;

    if (argc < 4) {
        serial_printf(
            "%s: usage: %s <image> <bytes per block> <reserved blocks factor>\n", argv[0],
            argv[0]);
        return 1;
    }
    image = fopen(drive_first, R);
    if (image == NULL) {
        serial_printf("%s: error: no valid image specified.\n", argv[0]);
    }

    fseek(image, 0L, SEEK_END);
    uint64_t imgsize = (uint64_t)ftell(image);
    rewind(image);
    fclose(image);
    image = fopen(drive_first, W);

    serial_printf("%s: info: formatting %lu bytes...\n", argv[0], imgsize);

    uint64_t bytesperblock = 512;

    if ((bytesperblock <= 0) || (bytesperblock % 512)) {
        serial_printf("%s: error: block size MUST be a multiple of 512.\n", argv[0]);
        fclose(image);
        return 1;
    }

    if (imgsize % bytesperblock) {
        serial_printf("%s: error: image is not block-aligned.\n", argv[0]);
        fclose(image);
        return 1;
    }

    uint64_t reserved_factor = 16;

    if ((reserved_factor <= 0) || (reserved_factor >= 100)) {
        serial_printf("%s: error: reserved blocks factor must be between 1%% and 99%%\n",
                      argv[0]);
        fclose(image);
        return 1;
    }

    uint64_t blocks = imgsize;

    fseek(image, 0, SEEK_SET);
    fwrite(boot_sector, 512, 1, image);

    fseek(image, 4, SEEK_SET);
    fputs("_ECH_FS_", image);
    wr_qword(12, blocks);                           // blocks
    wr_qword(20, blocks / (100 / reserved_factor)); // reserved blocks
    wr_qword(28, bytesperblock);                    // block size
    // mark reserved blocks
    uint64_t loc = RESERVED_BLOCKS * bytesperblock;

    uint64_t fatsize = (blocks * sizeof(uint64_t)) / bytesperblock;
    uint64_t dirsize = blocks / (100 / reserved_factor);

    for (uint64_t i = 0; i < (RESERVED_BLOCKS + fatsize + dirsize); i++) {
        wr_qword(loc, 0xfffffffffffffff0);
        loc += sizeof(uint64_t);
    }
    fflush(image);
    fclose(image);
    return 0;
}
