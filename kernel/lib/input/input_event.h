#pragma once
#include <types.h>
#include <resources/keyboard_enum.h>


enum input_event_type {
    EVENT_KEYBOARD,
    EVENT_MOUSE,
};
enum input_event_keyboard_type {
    KEYBOARD_KEY_DOWN,
    KEYBOARD_KEY_UP,
};
struct input_event_keyboard_event {
    enum input_event_keyboard_type s_type;
    enum keycode_enum keycode;   
};
typedef struct {
    enum input_event_type type;
    union {
        struct input_event_keyboard_event keyboard;
    };
} input_event_t;


void input_event_append_event(input_event_t event);
input_event_t* input_event_get_event();


