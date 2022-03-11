#include "input_device.h"
#include <string.h>
#include <types.h>
input_device_t devices[64];
uint8_t device_count = 0;

/**
 * @return uint8_t device "id"
 */
uint8_t input_device_create_device(char* name, char* type, char keymap[512], void (*handler)(uint8_t)) {
    devices[device_count].name = name;
    devices[device_count].type = type;
    for (int i = 0; i < 512; i++) {
        devices[device_count].keymap[i] = keymap[i];
    }

    devices[device_count].handler = handler;
    device_count++;
    return device_count - 1;
}
void input_device_set_keymap(uint8_t device_id, char keymap[512]) {
    for (int i = 0; i < 512; i++) {
        devices[device_id].keymap[i] = keymap[i];
    }
}
input_device_info_t input_device_get_info() {
    input_device_info_t info;

    for (int i = 0; i < device_count; i++) {
        info.id[i] = i;
        info.name[i] = devices[i].name;
        info.type[i] = devices[i].type;
        for (int j = 0; j < 512; j++) {
            info.keymap[i][j] = devices[i].keymap[j];
        }
    }
    return info;
}

input_device_info_t input_device_of_type_get_info(char* type) {
    input_device_info_t info;

    for (int i = 0; i < device_count; i++) {
        if (strcmp(devices[i].type, type) == 0) {
            info.id[i] = i;
            info.name[i] = devices[i].name;
            info.type[i] = devices[i].type;
            for (int j = 0; j < 512; j++) {
                info.keymap[i][j] = devices[i].keymap[j];
            }
        }
    }
    return info;
}

void input_device_send_key(uint8_t device_id, uint8_t key) {
    devices[device_id].handler(devices[device_id].keymap[key - 1]);
}
