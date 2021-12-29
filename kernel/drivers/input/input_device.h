#include <types.h>


/**
 * @name: name of the input_reader
 * @callback: callback function to be called when a key is pressed
 * @used: defines if listener is used or can be reassigned
 */
struct listener {
    char* name;
    void (*callback)(uint8_t);
    bool used = false;
};

/**
 * @name: name for identification and searching
 * @type: type of device (mouse, keyboard, joystick, ...)
 * @keymap: keymap for device, input bit to standartized keycode (e.g. keymap[0] is "a" key so: keymap[0] = 0x1)])
 * @listeners: array of function pointers that will be called when key is pressed
 */
struct input_device {
    char* name;
    char* type;
    
    uint8_t keymap[512];

    struct listener listeners[64];
    
};