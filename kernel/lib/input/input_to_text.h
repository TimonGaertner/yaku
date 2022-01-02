#pragma once

#include <types.h>

void (*input_to_text_on_enter)(char*);
void (*input_to_text_on_string_change)(char*);
void input_to_text_init();