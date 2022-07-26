#pragma once
#include <lib/fuse.h>
#include <types.h>

/*

Es sollen Endpoints created werden können. Diese sollen in Ordnern strukturiert werden
können. Jeder endpoint besitzt ein Fuse_operations struct. Es sollen die ganz normalen
Fuse Operations möglich sein. Diese werden dann an die endpoint fuse_operations
weitergegeben, dabei wird der teil des paths gestrichen, welcher zum endpoint führt.

*/
enum entry_type { ENTRY_TYPE_DIR, ENTRY_TYPE_ENDPOINT };
enum endpoint_type { ENDPOINT_TYPE_DIRECTORY, ENDPOINT_TYPE_FILE };
struct virtual_fs_directory_entry {
    char name[256];
    enum entry_type type;
    uint64_t* pointer;
};

struct virtual_fs_directory {
    uint32_t entries_count;
    struct virtual_fs_directory_entry* entries;
};
struct virtual_fs_endpoint {
    struct fuse_operations fuse_ops;
    enum endpoint_type type;
};

uint8_t virtual_fs_create_endpoint(struct fuse_operations* fuse_operations,
                                   enum endpoint_type endpoint_type, char* path);
uint8_t virtual_fs_create_directory(char* path);
uint8_t virtual_fs_init();
