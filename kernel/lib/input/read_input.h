#pragma once
#include <types.h>

void handle_keystroke(uint8_t keystroke, uint8_t *buffer);

/**
 * @buffer: array in which key"events" will be stored
 */
typedef struct read_input_listener{
    uint8_t buffer[256];
    void *handle_keystroke;
    uint8_t current_buffer_pointer;
    uint8_t last_read_buffer_pointer;    
} read_input_listener;