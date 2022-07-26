#include "virtual_fs.h"
#include <drivers/serial.h>
#include <lib/fuse.h>
#include <string.h>
#include <types.h>

struct virtual_fs_directory_entry* virtual_fs_root;

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
    memset(result, 0, sizeof(struct create_fs_directory_entry_path_result));
    result->parent = virtual_fs_root;
    result->name = NULL;
    if (path[0] == '/') {
        path++;
    }
    char* name = strtok(path, "/");
    while (name != NULL) {
        struct virtual_fs_directory_entry* entry =            virtual_fs_get_directory_entry(result->parent->pointer, name);
        if (entry == NULL) {
            break;
        }
        result->parent = entry;
        name = strtok(NULL, "/");
    }
    if (name != NULL) {
        result->name = name;
    }
    return result;
}

uint8_t virtual_fs_add_directory_entry(struct virtual_fs_directory* parent_directory,
                                       char* name, enum entry_type entry_type,
                                       uint64_t* pointer) {
    parent_directory->entries_count++;
    parent_directory->entries =
        realloc(parent_directory->entries, sizeof(struct virtual_fs_directory_entry) *
                                               parent_directory->entries_count);
    struct virtual_fs_directory_entry* entry =
        &parent_directory->entries[parent_directory->entries_count - 1];
    strcpy(entry->name, name);
    entry->type = entry_type;
    entry->pointer = pointer;
    serial_printf("Added entry %s\n", entry->name);
    return 0;
}

uint8_t virtual_fs_create_directory(char* path) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path);
    serial_printf("Creating directory %s\n", result->name);
    serial_printf("Parent is %p\n", result->parent);
    if (result->parent == NULL) {
        return 1;
    }
    if (result->name == NULL) {
        return 1;
    }
    if (result->parent->type != ENTRY_TYPE_DIR) {
        return 1;
    }
    if (virtual_fs_get_directory_entry(result->parent->pointer, result->name) != NULL) {
        return 1;
    }
    struct virtual_fs_directory* directory = malloc(sizeof(struct virtual_fs_directory));
    memset(directory, 0, sizeof(struct virtual_fs_directory));
    virtual_fs_add_directory_entry(result->parent->pointer, result->name, ENTRY_TYPE_DIR,
                                   directory);
    free(result);
    return 0;
}
uint8_t virtual_fs_create_endpoint(struct fuse_operations* fuse_operations,
                                   enum endpoint_type endpoint_type, char* path) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path);
    serial_printf("%s\n", path);
    serial_printf("parent %s\n", result->parent->name);
    serial_printf("name %s\n", result->name);
    if (result->name == NULL || result->parent == NULL) {
        return 1;
    }
    struct virtual_fs_endpoint* endpoint = malloc(sizeof(struct virtual_fs_endpoint));
    memcpy(&endpoint->fuse_ops, fuse_operations, sizeof(struct fuse_operations));
    endpoint->type = endpoint_type;
    virtual_fs_add_directory_entry(result->parent->pointer, result->name,
                                   ENTRY_TYPE_ENDPOINT, (uint64_t*)endpoint);
    free(result);
    return 0;
}
struct fuse_operations virtual_fs_operations = {};

uint8_t virtual_fs_init() {
    virtual_fs_root = malloc(sizeof(struct virtual_fs_directory_entry));
    strcpy(virtual_fs_root->name, "root");
    virtual_fs_root->type = ENTRY_TYPE_DIR;
    virtual_fs_root->pointer = malloc(sizeof(struct virtual_fs_directory));
    memset(virtual_fs_root->pointer, 0, sizeof(struct virtual_fs_directory));
}
