#include "mouse_handler.h"
#include <types.h>
#include <drivers/serial.h>

uint8_t mouse_handler_byte_count = 1;
uint8_t mouse_handler_byte_buffer[3];

bool mouse_handler_middle_button_pressed = false;
bool mouse_handler_right_button_pressed = false;
bool mouse_handler_left_button_pressed = false;

bool mouse_handler_y_negative = false;
bool mouse_handler_x_negative = false;

int32_t mouse_handler_x_delta = 0;
int32_t mouse_handler_y_delta = 0;

void mouse_handler(uint8_t byte) {
    if (mouse_handler_byte_count == 1 && (byte & (1 << 3))) {
        if (byte & (1 << 5)) {
            mouse_handler_y_negative = true;
        }
        if (byte & (1 << 4)) {
            mouse_handler_x_negative = true;
        }

        if (!mouse_handler_middle_button_pressed && (byte & (1 << 2))) {
            mouse_handler_middle_button_pressed = true;
            input_event_t event;
            event.kind = EVENT_MOUSE_BUTTON;
            event.mouse_button.s_kind = MOUSE_BUTTON_DOWN;
            event.mouse_button.button = MOUSE_BUTTON_MIDDLE;
            input_event_append_event(event);
        } else if (mouse_handler_middle_button_pressed && !(byte & (1 << 2))) {
            mouse_handler_middle_button_pressed = false;
            input_event_t event;
            event.kind = EVENT_MOUSE_BUTTON;
            event.mouse_button.s_kind = MOUSE_BUTTON_UP;
            event.mouse_button.button = MOUSE_BUTTON_MIDDLE;
            input_event_append_event(event);
        }
        if (!mouse_handler_right_button_pressed && (byte & (1 << 1))) {
            mouse_handler_right_button_pressed = true;
            input_event_t event;
            event.kind = EVENT_MOUSE_BUTTON;
            event.mouse_button.s_kind = MOUSE_BUTTON_DOWN;
            event.mouse_button.button = MOUSE_BUTTON_RIGHT;
            input_event_append_event(event);
        } else if (mouse_handler_right_button_pressed && !(byte & (1 << 1))) {
            mouse_handler_right_button_pressed = false;
            input_event_t event;
            event.kind = EVENT_MOUSE_BUTTON;
            event.mouse_button.s_kind = MOUSE_BUTTON_UP;
            event.mouse_button.button = MOUSE_BUTTON_RIGHT;
            input_event_append_event(event);
        }
        if (!mouse_handler_left_button_pressed && (byte & (1 << 0))) {
            mouse_handler_left_button_pressed = true;
            input_event_t event;
            event.kind = EVENT_MOUSE_BUTTON;
            event.mouse_button.s_kind = MOUSE_BUTTON_DOWN;
            event.mouse_button.button = MOUSE_BUTTON_LEFT;
            input_event_append_event(event);
        } else if (mouse_handler_left_button_pressed && !(byte & (1 << 0))) {
            mouse_handler_left_button_pressed = false;
            input_event_t event;
            event.kind = EVENT_MOUSE_BUTTON;
            event.mouse_button.s_kind = MOUSE_BUTTON_UP;
            event.mouse_button.button = MOUSE_BUTTON_LEFT;
            input_event_append_event(event);
        }
    }
    if (mouse_handler_byte_count == 2) {
        if (byte > 0) {
            if (mouse_handler_x_negative) {
                mouse_handler_x_delta = -(int32_t)byte;
            }
            else {
                mouse_handler_x_delta = (int32_t)byte;
            }
        }
    }
    if (mouse_handler_byte_count == 3) {
        if (byte > 0) {
            if (mouse_handler_y_negative) {
                mouse_handler_y_delta = -(int32_t)byte;
            }
            else {
                mouse_handler_y_delta = (int32_t)byte;
            }
        }
        if (mouse_handler_x_delta != 0 || mouse_handler_y_delta != 0) {
            input_event_t event;
            event.kind = EVENT_MOUSE_MOTION;
            event.mouse_motion.x_rel = mouse_handler_x_delta;
            event.mouse_motion.y_rel = mouse_handler_y_delta;
            input_event_append_event(event);
            mouse_handler_x_delta = 0;
            mouse_handler_y_delta = 0;
        }
        mouse_handler_byte_count = 1;
        mouse_handler_x_negative = false;
        mouse_handler_y_negative = false;
        return;
    }
    if (mouse_handler_byte_count == 4) {

        if (byte != 0){
            // todo: implement wheel
        }
        // mouse_handler_byte_count = 1;
        // mouse_handler_x_negative = false;
        // mouse_handler_y_negative = false;
        // return;

    }
    mouse_handler_byte_buffer[mouse_handler_byte_count] = byte;
    mouse_handler_byte_count++;
}