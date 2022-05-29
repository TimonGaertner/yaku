#include "lba.h"
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <io.h>
#include <types.h>
static bool primary_controller_present = false;
static bool primary_controller_first_drive_present = false;
static bool primary_controller_second_drive_present = false;
static bool secondary_controller_present = false;
static bool secondary_controller_first_drive_present = false;
static bool secondary_controller_second_drive_present = false;
void lba_init() {
    io_outb(0x176, 0xA0);
    io_outb(0x172, 0x00);
    io_outb(0x173, 0x00);
    io_outb(0x174, 0x00);
    io_outb(0x175, 0x00);
    io_outb(0x177, 0xEC);
    timer_sleep_ticks(40);
    uint8_t status = io_inb(0x177);
    serial_printf("status: %x\n", status);
    // detect present ide controllers
    io_outb(LOW_LBA_PORT_PRIMARY, 0x88);
    if (io_inb(LOW_LBA_PORT_PRIMARY) == 0x88) {
        primary_controller_present = true;
        // test if first drive is present
        io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xA0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0x40) {
            primary_controller_first_drive_present = true;
        }
        // test if second drive is present
        io_outb(LBA_PRIMARY_DRIVE_SELECT_PORT, 0xB0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT) & 0x40) {
            primary_controller_second_drive_present = true;
        }
    }
    io_outb(LOW_LBA_PORT_SECONDARY, 0x88);
    if (io_inb(LOW_LBA_PORT_SECONDARY) == 0x88) {
        secondary_controller_present = true;
        // test if first drive is present
        io_outb(LBA_SECONDARY_DRIVE_SELECT_PORT, 0xA0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0x40) {
            secondary_controller_first_drive_present = true;
        }
        // test if second drive is present
        io_outb(LBA_SECONDARY_DRIVE_SELECT_PORT, 0xB0);
        timer_sleep_ticks(100);
        if (io_inb(LBA_SECONDARY_CONTROLLER_STATUS_PORT) & 0x40) {
            secondary_controller_second_drive_present = true;
        }
    }
    serial_printf("primary controller present: %d\n", primary_controller_present);
    serial_printf("primary controller first drive present: %d\n",
                  primary_controller_first_drive_present);
    serial_printf("primary controller second drive present: %d\n",
                  primary_controller_second_drive_present);
    serial_printf("secondary controller present: %d\n", secondary_controller_present);
    serial_printf("secondary controller first drive present: %d\n",
                  secondary_controller_first_drive_present);
    serial_printf("secondary controller second drive present: %d\n",
                  secondary_controller_second_drive_present);
}
void lba_read(uint8_t controller, uint8_t drive) {
    uint64_t base_port = 0;
    if (controller == 1) {
        base_port = LBA_PRIMARY_CONTROLLER_PORT_BASE;
    } else if (controller == 2) {
        base_port = LBA_SECONDARY_CONTROLLER_PORT_BASE;
    }
    io_outb(base_port + 1, 0x00);
    io_outb(base_port + 1, 0x00);
    io_outb(base_port + 2, 0x00);
    io_outb(base_port + 2, 0x01);
    io_outb(base_port + 3, (unsigned char)(1 >> 24));
    io_outb(base_port + 3, (unsigned char)(1));
    io_outb(base_port + 4, (unsigned char)(1 >> 32));
    io_outb(base_port + 4, (unsigned char)(1 >> 8));
    io_outb(base_port + 5, (unsigned char)(1 >> 40));
    io_outb(base_port + 5, (unsigned char)(1 >> 16));
    io_outb(base_port + 6, 0x40 | (1 << 4));
    io_outb(base_port + 7, 0x24);
    // uint32_t timeOut = 0;
    // while (!(io_inb(base_port+7) & 0x08) && timeOut++ < 100000) {
    //     serial_printf("waiting for drive to be ready %d\n", io_inb(base_port+7));
    // };
        serial_printf("hallo\n");
    for (uint16_t idx = 0; idx < 256; idx++) {
        uint64_t input = io_inb(base_port + 0);
        serial_printf("%d\n", input);
    }
}
void lba_write(uint8_t controller, uint8_t drive) {
    uint64_t base_port = 0;
    if (controller == 1) {
        base_port = LBA_PRIMARY_CONTROLLER_PORT_BASE;
    } else if (controller == 2) {
        base_port = LBA_SECONDARY_CONTROLLER_PORT_BASE;
    }
    io_outb(base_port + 1, 0x00);
    io_outb(base_port + 1, 0x00);
    io_outb(base_port + 2, 0x00);
    io_outb(base_port + 2, 0x01);
    io_outb(base_port + 3, (unsigned char)(1 >> 24));
    io_outb(base_port + 3, (unsigned char)(1));
    io_outb(base_port + 4, (unsigned char)(1 >> 32));
    io_outb(base_port + 4, (unsigned char)(1 >> 8));
    io_outb(base_port + 5, (unsigned char)(1 >> 40));
    io_outb(base_port + 5, (unsigned char)(1 >> 16));
    io_outb(base_port + 6, 0x40 | (1 << 4));
    io_outb(base_port + 7, 0x34);
    // uint32_t timeOut = 0;
    // while (!(io_inb(base_port+7) & 0x08) && timeOut++ < 100000) {
    //     serial_printf("waiting for drive to be ready %d\n", io_inb(base_port+7));
    // };
        serial_printf("hallo\n");
    for (uint16_t idx = 0; idx < 256; idx++) {
        io_outb(base_port + 0, 1);
    }
}
