#pragma once
#include <lib/fuse.h>
#include <types.h>
struct file_handle {
    struct fuse_file_info file_handle;
    uint64_t file_byte_ptr;
    char path[100];
    struct fuse_operations* operations;
} typedef file_handle_t;
