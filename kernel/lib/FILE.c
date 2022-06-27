#include "FILE.h"
#include <echfs/echfs-fuse.h>
#include <lib/dev/device_file.h>
#include <memory/pmm.h>
#include <string.h>
#include <types.h>

FILE* FILE_open(char* path, mode_t mode) {
    FILE* file = malloc(sizeof(FILE));
    char path_copy[strlen(path) + 1];
    strcpy(path_copy, path);
    char* token = strtok(path_copy, "/");
    if (strcmp(token, "dev") == 0) {
        path += 3;
        if (strncmp(path, "/", 1) == 0) {
            path += 1;
        }
        dev_dir_get_fuse_ops(path, &file->fuse_ops);
        file->fuse_ops.open(path, &file->file_info);
        return file;
    }

    echfs_get_fuse_operations(&file->fuse_ops);
    file->fuse_ops.open(path, &file->file_info);
    return file;
}
