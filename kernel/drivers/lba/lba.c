#include "lba.h"
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <io.h>
#include <types.h>
#include <write_to_drive.h>
static bool controllers_present[2] = {false, false};
static bool primary_controller_drives_present[2] = {false, false};
static bool secondary_controller_drives_present[2] = {false, false};
static uint64_t primary_controller_drive_size[2] = {0, 0};
static uint64_t secondary_controller_drive_size[2] = {0, 0};

//returns drive size in sectors (512 bytes)
uint64_t get_drive_size(enum ide_controller controller, enum drives drive) {
    if (controller == primary_controller) {
        return primary_controller_drive_size[drive];
    }
    if (controller == secondary_controller) {
        return secondary_controller_drive_size[drive];
    }
    return NULL;
}
void lba_init() {
    // detect present ide controllers
    io_outb(LOW_LBA_PORT_PRIMARY, 0x88);
    if (io_inb(LOW_LBA_PORT_PRIMARY) == 0x88) {
        controllers_present[0] = true;
        // test if first drive is present
        io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xA0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0x40) {
            primary_controller_drives_present[0] = true;
            uint16_t buffer[256];
            if (lba_identify(primary_controller, first_drive, &buffer[0])) {
                primary_controller_drive_size[0] =
                    (uint64_t)buffer[100] << 48 | (uint64_t)buffer[101] << 32 |
                    (uint64_t)buffer[102] << 16 | (uint64_t)buffer[103];
            }
        }
        // test if second drive is present
        io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xB0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0x40) {
            primary_controller_drives_present[1] = true;
            uint16_t buffer[256];
            if (lba_identify(primary_controller, second_drive, &buffer[0])) {
                primary_controller_drive_size[0] =
                    (uint64_t)buffer[100] << 48 | (uint64_t)buffer[101] << 32 |
                    (uint64_t)buffer[102] << 16 | (uint64_t)buffer[103];
            }
        }
    }
    io_outb(LOW_LBA_PORT_SECONDARY, 0x88);
    if (io_inb(LOW_LBA_PORT_SECONDARY) == 0x88) {
        controllers_present[1] = true;
        // test if third drive is present
        io_outb(LBA_SECONDARY_DRIVE_SELECT_PORT, 0xA0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0x40) {
            secondary_controller_drives_present[0] = true;
            uint16_t buffer[256];
            if (lba_identify(secondary_controller, first_drive, &buffer[0])) {
                primary_controller_drive_size[0] =
                    (uint64_t)buffer[100] << 48 | (uint64_t)buffer[101] << 32 |
                    (uint64_t)buffer[102] << 16 | (uint64_t)buffer[103];
            }
        }
        // test if fourth drive is present
        io_outb(LBA_SECONDARY_DRIVE_SELECT_PORT, 0xB0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0x40) {
            secondary_controller_drives_present[1] = true;
            uint16_t buffer[256];
            if (lba_identify(secondary_controller, second_drive, &buffer[0])) {
                primary_controller_drive_size[0] =
                    (uint64_t)buffer[100] << 48 | (uint64_t)buffer[101] << 32 |
                    (uint64_t)buffer[102] << 16 | (uint64_t)buffer[103];
            }
        }
    }
    serial_printf("%lu\n", primary_controller_drive_size[0]);
    serial_printf("controllers present: %d %d\n", controllers_present[0],
                  controllers_present[1]);
    serial_printf("primary drives present: %d %d\n",
    primary_controller_drives_present[0],
                  primary_controller_drives_present[1]);
    serial_printf("secondary drives present: %d %d\n",
    secondary_controller_drives_present[0], secondary_controller_drives_present[1]);
}
bool drive_present(enum ide_controller controller, enum drives drive) {
    if (controller == primary_controller) {
        return primary_controller_drives_present[drive];
    }
    if (controller == secondary_controller) {
        return secondary_controller_drives_present[drive];
    }
    return NULL;
}
// send identify command to a target drive
//  buffer = &(uint16_t buffer[256]);
/*     uint16_t 0: is useful if the device is not a hard disk.

    uint16_t 83: Bit 10 is set if the drive supports LBA48 mode.

    uint16_t 88: The bits in the low byte tell you the supported UDMA modes, the upper
   byte tells you which UDMA mode is active. If the active mode is not the highest
   supported mode, you may want to figure out why. Notes: The returned uint16_t should
   look like this in binary: 0000001 00000001. Each bit corresponds to a single mode. E.g.
   if the decimal number is 257, that means that only UDMA mode 1 is supported and running
   (the binary number above) if the binary number is 515, the binary looks like this,
   00000010 00000011, that means that UDMA modes 1 and 2 are supported, and 2 is running.
   This is true for every mode. If it does not look like that, e.g 00000001 00000011, as
   stated above, you may want to find out why. The formula for finding out the decimal
   number is 257 * 2 ^ position + 2 ^position - 1.

    uint16_t 93 from a master drive on the bus: Bit 11 is supposed to be set if the drive
   detects an 80 conductor cable. Notes: if the bit is set then 80 conductor cable is
   present and UDMA modes > 2 can be used; if bit is clear then there may or may not be an
   80 conductor cable and UDMA modes > 2 shouldn't be used but might work fine. Because
   this bit is "master device only", if there is a slave device and no master there is no
   way information about cable type (and would have to assume UDMA modes > 2 can't be
   used).

    uint16_t 60 & 61 taken as a uint32_t contain the total number of 28 bit LBA
   addressable sectors on the drive. (If non-zero, the drive supports LBA28.)

    uint16_t 100 through 103 taken as a uint64_t contain the total number of 48 bit
   addressable sectors on the drive. (Probably also proof that LBA48 is supported.) */
// void lba48_write(uint64_t lba, uint16_t count, uint16_t* buffer) {
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 1, 0x00);
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 1, 0x00);
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 2, 0x00);
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 2, count);
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 3, (unsigned char)(lba >> 24));
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 3, (unsigned char)(lba));
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 4, (unsigned char)(lba >> 32));
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 4, (unsigned char)(lba >> 8));
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 5, (unsigned char)(lba >> 40));
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 5, (unsigned char)(lba >> 16));
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 6, 0x40 | (unsigned char)(1 << 4));
//     io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 7, 0x34);
//     while (!(inb(0x1F7) & 0x08)) {}
//     for (uint16_t idx = 0; idx < 256; idx++) {

//         uint16_t = buffer[8 + idx * 2] | (buffer[8 + idx * 2 + 1] << 8);

//         outw(0x1F0, tmpword);
//     }
// }
bool lba_identify(enum ide_controller controller, enum drives drive, uint16_t* buffer) {
    if (controller == primary_controller) {
        if (drive == first_drive) {
            io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xA0);
        }
        if (drive == second_drive) {
            io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xB0);
        }
        io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 2, 0);
        io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 3, 0);
        io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 4, 0);
        io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 5, 0);
        io_outb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 7, 0xEC);
        timer_sleep_ticks(40);
        uint64_t status = io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT);
        if (status == 0) {
            return false; // drive does not exist
        }
        while (io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0x80) {}
        if (io_inb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 4) != 0 ||
            io_inb(LBA_PRIMARY_CONTROLLER_PORT_BASE + 5) != 0) {
            return false; // drive is not ata
        }
        while (!(io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0b1000) &&
               !(io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0b1)) {}

        if (io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0b1) {
            return false; // error
        }
        for (int i = 0; i < 256; i++) {
            buffer[i] = io_inw(LBA_PRIMARY_CONTROLLER_PORT_BASE);
        }
        return true;
    } else if (controller == secondary_controller) {
        if (drive == first_drive) {
            io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xA0);
        }
        if (drive == second_drive) {
            io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xB0);
        }
        io_outb(LBA_SECONDARY_CONTROLLER_PORT_BASE + 2, 0);
        io_outb(LBA_SECONDARY_CONTROLLER_PORT_BASE + 3, 0);
        io_outb(LBA_SECONDARY_CONTROLLER_PORT_BASE + 4, 0);
        io_outb(LBA_SECONDARY_CONTROLLER_PORT_BASE + 5, 0);
        io_outb(LBA_SECONDARY_CONTROLLER_PORT_BASE + 7, 0xEC);
        timer_sleep_ticks(40);
        uint64_t status = io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT);
        if (status == 0) {
            return false; // drive does not exist
        }
        while (io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0x80) {}
        if (io_inb(LBA_SECONDARY_CONTROLLER_PORT_BASE + 4) != 0 || io_inb(0x1F5) != 0) {
            return false; // drive is not ata
        }
        while (!(io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0b1000) &&
               !(io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0b1)) {}

        if (io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0b1) {
            return false; // error
        }
        for (int i = 0; i < 256; i++) {
            buffer[i] = io_inw(LBA_SECONDARY_CONTROLLER_PORT_BASE);
        }
        return true;
    }
}
