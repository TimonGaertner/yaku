#include "virtual_fs.h"
#include <lib/fuse.h>
#include <string.h>
#include <types.h>

struct virtual_fs_directory* virtual_fs_root;

struct path_result {
    struct virtual_fs_directory_entry* parent;
    struct virtual_fs_directory_entry* endpoint; // Null if no endpoint
    char* endpoint_path_to_be_passed;            // Null if no endpoint
};
struct create_fs_directory_entry_path_result {
    struct virtual_fs_directory_entry* parent;
    char* name;
};

struct virtual_fs_directory_entry*
virtual_fs_get_directory_entry(struct virtual_fs_directory* directory, char* name) {
    for (uint32_t i = 0; i < directory->entries_count; i++) {
        if (strcmp(directory->entries[i].name, name) == 0) {
            return &directory->entries[i];
        }
    }
    return NULL;
}
struct create_fs_directory_entry_path_result* create_fs_entry_path_resolver(char* path) {
    struct create_fs_directory_entry_path_result* result =
        malloc(sizeof(struct create_fs_directory_entry_path_result));
    result->parent = NULL;
    result->name = NULL;
    if (path[0] == '/') {
        path++;
    }
    char* name = strtok(path, "/");
    while (name != NULL) {
        if (result->parent == NULL) {
            result->parent = virtual_fs_get_directory_entry(virtual_fs_root, name);
        } else {
            result->parent =
                virtual_fs_get_directory_entry(result->parent->pointer, name);
        }
        name = strtok(NULL, "/");
    }
    if (name != NULL) {
        result->name = name;
    }
    return result;
}
uint8_t virtual_fs_create_directory(char* path) {}
uint8_t virtual_fs_create_endpoint(struct fuse_operations* fuse_operations, char* path) {}
struct fuse_operations virtual_fs_operations = {};
