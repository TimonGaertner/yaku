#include "string.h"
#include <types.h>

bool strcmp(char* str1, char* str2) {
    while (*str1 == *str2) {
        if (*str1 == '\0') {
            return true;
        }
        str1++;
        str2++;
    }
    return false;
}