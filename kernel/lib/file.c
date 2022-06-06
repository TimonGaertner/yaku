#include "file.h"
#include <drivers/rtc.h>
#include <lib/datetime.h>
#include <memory/pmm.h>
#include <string.h>
struct FILE* fopen(char* filename, char* access_mode) {
    FILE* file = (struct drive_image*)malloc((sizeof(FILE) - 1) / 4069 + 1);
    file->access_mode = access_mode;
    file->name = filename;
    file->byte_ptr = 0;
    datetime_t rtc_time;
    rtc_read_time(&rtc_time);
    uint64_t tv_sec = (uint64_t)datetime_to_timestamp(&rtc_time, false);
    file->st.st_mtime.tv_sec = tv_sec;
    file->st.st_ctime.tv_sec = tv_sec;
    file->st.st_atime.tv_sec = tv_sec;
    file->st.st_size = 0;
    file->st.st_mode = 0;
    return file;
}
uint8_t fseek(FILE* drive_image, int64_t offset, enum origin origin) {
    if (origin == SEEK_SET) {
        if (offset < 0)
            return 1;
        drive_image->byte_ptr = offset;
    } else if (origin == SEEK_CUR) {
        if (drive_image->byte_ptr + offset < 0) {
            return 1;
        }
        drive_image->byte_ptr += offset;
    } else if (origin == SEEK_END) {
        if (drive_image->st.st_size + offset < 0) {
            return 1;
        }
        drive_image->byte_ptr = drive_image->st.st_size + offset;
    }
    return 0;
}
void rewind(FILE* drive_image) {
    drive_image->byte_ptr = 0;
}
void fclose(FILE* drive_image) {
    for (int i = 0; i < max_blocks; i++) {
        if (drive_image->data[i] != NULL) {
            free(drive_image->data[i], 1);
        }
    }
    free(drive_image, (sizeof(FILE) - 1) / 4069 + 1);
}
uint64_t ftell(FILE* drive_image) {
    return drive_image->byte_ptr;
}
uint8_t fwrite(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
               FILE* image) {
    if (image->access_mode[0] != 'w') {
        return 1;
    }
    uint64_t size = size_of_element * number_of_elements;
    uint64_t block_num = image->byte_ptr / 4096;
    uint64_t block_offset = image->byte_ptr % 4096;
    for (uint64_t i = block_offset; i < size; i += 1) {
        if (image->data[block_num] == NULL) {
            image->data[block_num] = (uint8_t*)malloc(4096);
        }
        image->data[block_num][i] = ptr[i - block_offset];
        image->byte_ptr += 1;
        if (image->byte_ptr > image->st.st_size) {
            image->st.st_size = image->byte_ptr;
        }
        if (image->byte_ptr % 4096 == 0) {
            block_num += 1;
        }
    }
    return 0;
}
uint8_t fputs(char* str, FILE* image){
    if (image->access_mode[0] != 'w') {
        return 1;
    }
    uint64_t size = strlen(str);
    uint64_t block_num = image->byte_ptr / 4096;
    uint64_t block_offset = image->byte_ptr % 4096;
    for (uint64_t i = block_offset; i < size; i += 1) {
        if (image->data[block_num] == NULL) {
            image->data[block_num] = (uint8_t*)malloc(4096);
        }
        image->data[block_num][i] = str[i - block_offset];
        image->byte_ptr += 1;
        if (image->byte_ptr > image->st.st_size) {
            image->st.st_size = image->byte_ptr;
        }
        if (image->byte_ptr % 4096 == 0) {
            block_num += 1;
        }
    }
    return 0;
}
uint8_t fread(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
              FILE* image) {

    uint64_t size = size_of_element * number_of_elements;
    uint64_t block_num = image->byte_ptr / 4096;
    uint64_t block_offset = image->byte_ptr % 4096;
    for (uint64_t i = block_offset; i < size; i += 1) {
        if (image->data[block_num] == NULL) {
            ptr[i - block_offset] = 0;
        }
        ptr[i - block_offset] = image->data[block_num][i];
        image->byte_ptr += 1;
        if (image->byte_ptr % 4096 == 0) {
            block_num += 1;
        }
    }
    return 0;
}
uint8_t fflush(FILE* image) {
    return 0;
}
uint8_t fgetc(FILE* image){
    uint64_t block_num = image->byte_ptr / 4096;
    uint64_t block_offset = image->byte_ptr % 4096;
    if (image->data[block_num] == NULL) {
        return 0;
    }
    image->byte_ptr += 1;
    return image->data[block_num][block_offset];
}
