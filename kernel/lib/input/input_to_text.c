#include "input_to_text.h"

#include <drivers/serial.h>
#include <keyboard_stringmap.h>
#include <lib/input/read_input.h>
#include <string.h>
#include <types.h>

bool input_to_text_enter_callback = false;
bool input_to_text_string_change_callback = false;

char* input_to_text_field = "";
read_input_listener input_to_text_listener;

bool input_to_text_shift_pressed = false;
bool input_to_text_caps_pressed = false;
bool input_to_text_altgr_pressed = false;

bool function_key = false; // next key is a key like: alt_gr, delete, and other keys that send multiple bytes

uint32_t cursor = 0;

void input_to_text_handle_keystroke(char keystroke, read_input_listener* this) {
    serial_printf("1:%d\n", keystroke);
    serial_printf(": %s\n", input_to_text_field);
    if (keystroke < 0) {
        if (keystroke == -74) {
            input_to_text_shift_pressed = false;
        }

        return;
    }
    if (keystroke > 105) {
        return;
    }
    if (keystroke == 105) {
        function_key = true;
        return;
    }
    char* keystroke_str[3] = {keyboard_stringmap[keystroke][0],
                              keyboard_stringmap[keystroke][1],
                              keyboard_stringmap[keystroke][2]};

    if (keystroke_str == NULL) {
        return;
    }
    if (strcmp("", keystroke_str[0])) {
        return;
    }
    if (strcmp("SHIFT", keystroke_str[0])) {
        input_to_text_shift_pressed = true;
        return;
    }
    if (strcmp("ALT_GR", keystroke_str[0]) && function_key == true) {
        input_to_text_altgr_pressed = true;
        return;
    }
    if (strcmp("CAPS", keystroke_str[0])) {
        input_to_text_caps_pressed = !input_to_text_caps_pressed;
        return;
    }
    if (strcmp("ENTER", keystroke_str[0])) {
        if (input_to_text_enter_callback) {
            input_to_text_on_enter(input_to_text_field);
        }
        return;
    }
    if (strcmp("ALT", keystroke_str[0])) {
        return;
    }
    if (strcmp("CTRL", keystroke_str[0])) {
        return;
    }
    if (strcmp("DEL", keystroke_str[0])) {
        return;
    }

    if (strcmp("BACKSPACE", keystroke_str[0])) {
        if (strlen(input_to_text_field) > 0) {
            if (cursor != strlen(input_to_text_field)) {
                for (int i = cursor; i < strlen(input_to_text_field); i++) {
                    input_to_text_field[i] = input_to_text_field[i + 1];
                }
                cursor--;
                if (input_to_text_string_change_callback) {
                    input_to_text_on_string_change(input_to_text_field);
                }
                return;
            }
            if (cursor == strlen(input_to_text_field)) {
                input_to_text_field[cursor - 1] = "\0";
                cursor--;
                if (input_to_text_string_change_callback) {
                    input_to_text_on_string_change(input_to_text_field);
                }
                return;
            }
        }
    }
    if (strcmp("SPACE", keystroke_str[0])) {
        input_to_text_field = strcat_inbetween(input_to_text_field, " ", cursor);
        cursor++;
        if (input_to_text_string_change_callback) {
            input_to_text_on_string_change(input_to_text_field);
        }
        return;
    }
    if (strcmp("TAB", keystroke_str[0])) {
        input_to_text_field = strcat_inbetween(input_to_text_field, "\t", cursor);
        if (input_to_text_string_change_callback) {
            input_to_text_on_string_change(input_to_text_field);
        }
        cursor++;
        return;
    }
    if (input_to_text_altgr_pressed) {
        input_to_text_field =
            strcat_inbetween(input_to_text_field, keystroke_str[2], cursor);
        cursor++;
        if (input_to_text_string_change_callback) {
            input_to_text_on_string_change(input_to_text_field);
        }
        return;
    }
    if (input_to_text_shift_pressed) {
        if (input_to_text_caps_pressed) {
            input_to_text_field =
                strcat_inbetween(input_to_text_field, keystroke_str[0], cursor);
            cursor++;
            if (input_to_text_string_change_callback) {
                input_to_text_on_string_change(input_to_text_field);
            }
            return;
        }
        input_to_text_field =
            strcat_inbetween(input_to_text_field, keystroke_str[1], cursor);
        cursor++;
        if (input_to_text_string_change_callback) {
            input_to_text_on_string_change(input_to_text_field);
        }
        return;
    }
    if (input_to_text_caps_pressed) {
        input_to_text_field =
            strcat_inbetween(input_to_text_field, keystroke_str[1], cursor);
        cursor++;
        if (input_to_text_string_change_callback) {
            input_to_text_on_string_change(input_to_text_field);
        }
        return;
    }
    // strcat_inbetween, 2str becomes end of 1str
    input_to_text_field = strcat_inbetween(input_to_text_field, keystroke_str[0], cursor);
    cursor++;
    if (input_to_text_string_change_callback) {
        input_to_text_on_string_change(input_to_text_field);
    }
}

char* input_to_text_field_get() {
    return input_to_text_field;
}

void input_to_text_init() {
    input_to_text_field = "";
    read_input_init_listener(&input_to_text_listener);
    input_to_text_listener.keystroke_handler = &input_to_text_handle_keystroke;
}

void input_to_text_add_enter_callback(void (*callback)(char*)) {
    input_to_text_on_enter = &callback;
    input_to_text_enter_callback = true;
}

void input_to_text_add_string_change_callback(void (*callback)(char*)) {
    input_to_text_on_string_change = &callback;
    input_to_text_string_change_callback = true;
}