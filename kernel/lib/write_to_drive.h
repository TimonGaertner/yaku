#pragma once
#include <types.h>

enum drive { drive_first, drive_second, drive_third, drive_fourth };
enum access_mode { R, W };
enum origin { SEEK_SET, SEEK_CUR, SEEK_END };
struct drive_image {
    enum access_mode access_mode;
    enum drive drive;
    uint64_t byte_pointer_position;
};
struct drive_image* write_to_drive_fopen(enum drive drive, enum access_mode access_mode);
uint8_t write_to_drive_fseek(struct drive_image* drive_image, int64_t offset, enum origin origin);
void write_to_drive_rewind(struct drive_image* drive_image);
void write_to_drive_fclose(struct drive_image* drive_image);
uint64_t write_to_drive_ftell(struct drive_image* drive_image);
uint8_t write_to_drive_fwrite(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
               struct drive_image* image);
uint8_t write_to_drive_fread(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
              struct drive_image* image);
uint8_t write_to_drive_fputs(char* str, struct drive_image* image);
uint8_t write_to_drive_fflush(struct drive_image* image);
uint8_t write_to_drive_fgetc(struct drive_image* image);
