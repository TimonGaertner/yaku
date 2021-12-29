#include "input_device.h"
#include <types.h>

struct input_device devices[64];
uint8_t device_count = 0;

/**
 * @return uint8_t device "id" 
 */
uint8_t input_device_create_device(char* name, char* type, uint8_t keymap[512]) {
    devices[device_count].name = name;
    devices[device_count].type = type;

    for (int i = 0; i < 512; i++) {
        devices[device_count].keymap[i] = keymap[i];
    }

    device_count++;
    return device_count - 1;
}

/**
 * @return id used to remove listener
 */
uint8_t input_device_add_listener(uint8_t device_id, char* name, void (*callback)(uint8_t)) {
    for (uint8_t i = 0; i < 64; i++) {
        if (devices[device_id].listeners[i].used == false) {
            devices[device_id].listeners[i].name = name;
            devices[device_id].listeners[i].callback = callback;
            devices[device_id].listeners[i].used = true;
            return i;
        }
    }
}

void input_device_remove_listener(uint8_t device_id, uint8_t listener_id) {
    devices[device_id].listeners[listener_id].name = NULL;
    devices[device_id].listeners[listener_id].callback = NULL;
    devices[device_id].listeners[listener_id].used = false;
}

void input_device_send_key(uint8_t device_id, uint8_t key) {
    for (uint8_t i = 0; i < 64; i++) {
        if (devices[device_id].listeners[i].used == true) {
            devices[device_id].listeners[i].callback(devices[device_id].keymap[key]);
        }
    }
}