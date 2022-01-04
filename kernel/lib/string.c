#include "string.h"
#include <string.h>
#include <types.h>
#include <drivers/serial.h>

/**
 * @buffer: buffer to store the new string in. (minimal length: len(str1)+len(str2)+1)
 */
void strcat_inbetween(char* buffer, const char* str1, const char* str2, size_t index) {
    if (index >= strlen(str1)){
        strcat(buffer, str1);
        strcat(buffer, str2);
        return;
    }
    if (index < 0){
        index=0;
    }
    strncpy(buffer, str1, index);
    buffer[index] = '\0';
    strcat(buffer, str2);
    strcat(buffer, str1 + index);
}