#include "input_to_text.h"

#include <keyboard_stringmap.h>
#include <lib/input/read_input.h>
#include <string.h>
#include <types.h>

char* input_to_text_field = "";
read_input_listener input_to_text_listener;

bool input_to_text_shift_pressed = false;
bool input_to_text_caps_pressed = false;
bool input_to_text_altgr_pressed = false;

bool function_key = false;

uint32_t cursor = 0;

void input_to_text_handle_keystroke(uint8_t keystroke, read_input_listener* this) {
    if (keystroke < 0) {
        return;
    }
    if (keystroke == 105) {
        function_key = true;
        return;
    }

    char* keystroke_str = keyboard_stringmap[keystroke];
    if (keystroke_str == NULL) {
        return;
    }
    if (keystroke_str == "") {
        return;
    }

    if (keystroke_str[0] == 'SHIFT') {
        input_to_text_shift_pressed = true;
        return;
    }
    if (keystroke_str[0] == 'ALT' && function_key == true) {
        input_to_text_altgr_pressed = true;
        return;
    }
    if (keystroke_str[0] == 'CAPS') {
        input_to_text_caps_pressed = !input_to_text_caps_pressed;
        return;
    }
    if (keystroke_str[0] == 'ENTER') {
        input_to_text_on_enter_callback();
        return;
    }

    if (keystroke_str[0] == 'BACKSPACE') {
        if (strlen(input_to_text_field) > 0) {
            if (cursor != strlen(input_to_text_field)) {
                for (int i = cursor; i < strlen(input_to_text_field); i++) {
                    input_to_text_field[i] = input_to_text_field[i + 1];
                }
                return;
            }
            if (cursor == strlen(input_to_text_field)) {
                input_to_text_field[cursor - 1] = '\0';
                return;
            }            
        }
    }
    if (keystroke_str[0] == 'SPACE') {
        if (curso)
    }
}

void input_to_text_init(void* on_enter_callback) {
    input_to_text_field = "";
    read_input_init_listener(&input_to_text_listener);
    input_to_text_listener.keystroke_handler = &input_to_text_handle_keystroke;
    input_to_text_on_enter_callback = &on_enter_callback;
}
