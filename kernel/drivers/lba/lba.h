#pragma once
#include <types.h>


//primary controller from port 0x1F0 - 0x1F7
#define LBA_PRIMARY_CONTROLLER_PORT_BASE 0x1F0
#define LOW_LBA_PORT_PRIMARY 0x1F3
#define LBA_PRIMARY_DRIVE_SELECT_PORT 0x1F6
#define LBA_PRIMARY_CONTROLLER_PORT_STATUS_PORT 0x1F7
//secondary controller from port 0x170 - 0x177
#define LBA_SECONDARY_CONTROLLER_PORT_BASE 0x170
#define LOW_LBA_PORT_SECONDARY 0x173
#define LBA_SECONDARY_DRIVE_SELECT_PORT 0x176
#define LBA_SECONDARY_CONTROLLER_STATUS_PORT 0x177
void lba_init();
typedef enum {
    LBA_MASTER_DRIVE = 0xA0,
    LBA_SLAVE_DRIVE = 0xB0    
}lba_drives_t;
enum drives{
    first_drive,
    second_drive
}
enum ide_controller{
    primary_controller,
    secondary_controller
}

bool lba_identify(lba_drives_t drive, uint16_t* buffer);
bool drive_present(enum ide_controller controller, enum drives drive);
uint64_t get_master_drive_size();
