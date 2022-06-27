#pragma once
#include <types.h>
#include <lib/fuse.h>

typedef struct file_struct {
    struct fuse_operations fuse_ops;
    struct fuse_file_info file_info;
    uint64_t offset;
} FILE;
