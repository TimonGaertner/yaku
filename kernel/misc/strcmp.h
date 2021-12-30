#pragma once

#include <types.h>

static bool strcmp(char* a, char* b) {
    for (int i = 0; i < 512; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}