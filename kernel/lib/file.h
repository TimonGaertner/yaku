#pragma once
#include <lib/datetime.h>
#define max_blocks 2000 //block size=4096 bytes * 2000 = 8MB
struct time{
    uint64_t tv_sec;
};
struct stat{
    uint32_t st_mode;
    struct time st_mtime;
    struct time st_ctime;
    struct time st_atime;
    uint64_t st_size;
};

enum origin { SEEK_SET, SEEK_CUR, SEEK_END };
typedef struct file {
    struct stat st;
    char* name;
    uint64_t byte_ptr;
    uint8_t* data[max_blocks]; // ptrs to blocks
    char access_mode[3];
} FILE;

struct FILE* fopen(char* filename, char* access_mode);
uint8_t fseek(FILE* drive_image, int64_t offset, enum origin origin);
void rewind(FILE* drive_image);
void fclose(FILE* drive_image);
uint64_t ftell(FILE* drive_image);
uint8_t fwrite(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
               FILE* image);
uint8_t fread(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
              FILE* image);
uint8_t fputs(char* str, FILE* image);
uint8_t fflush(FILE* image);
uint8_t fgetc(FILE* image);
