#include "virtual_fs.h"
#include <drivers/serial.h>
#include <lib/fuse.h>
#include <string.h>
#include <types.h>

struct virtual_fs_directory_entry* virtual_fs_root;


struct create_fs_directory_entry_path_result {
    struct virtual_fs_directory_entry* parent;
    char name[MAX_NAME_LENGTH];
};


struct virtual_fs_handle {
    struct virtual_fs_directory_entry* endpoint;
    char path[MAX_PATH_LENGTH];
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
struct create_fs_directory_entry_path_result*
create_fs_entry_path_resolver(char* path, struct virtual_fs_directory_entry* parent) {
    struct create_fs_directory_entry_path_result* result =
        malloc(sizeof(struct create_fs_directory_entry_path_result));
    memset(result, 0, sizeof(struct create_fs_directory_entry_path_result));
    result->parent = parent;
    if (path[0] == '/') {
        path++;
        result->parent = virtual_fs_root;
    }
    if (result->parent == NULL) {
        result->parent = virtual_fs_root;
    }
    char* name = strtok(path, "/");
    while (name != NULL) {
        struct virtual_fs_directory_entry* entry =
            virtual_fs_get_directory_entry(result->parent->pointer, name);
        if (entry == NULL) {
            break;
        }
        result->parent = entry;
        name = strtok(NULL, "/");
    }
    if (name != NULL) {
        strcpy(result->name, name);
    }
    return result;
}
struct endpoint_path_result* virtual_fs_endpoint_path_resolver(char* path) {
    struct endpoint_path_result* result = malloc(sizeof(struct endpoint_path_result));
    memset(result, 0, sizeof(struct endpoint_path_result));
    result->endpoint = NULL;
    result->parent = virtual_fs_root;
    if (path[0] == '/') {
        path++;
    }
    char* name = strtok(path, "/");
    while (name != NULL) {
        struct virtual_fs_directory_entry* entry =
            virtual_fs_get_directory_entry(result->parent->pointer, name);
        if (entry == NULL) {
            break;
        }
        if (entry->type == ENTRY_TYPE_ENDPOINT) {
            result->endpoint = entry;
            name = strtok(NULL, "/");
            break;
        }
        result->parent = entry;
        name = strtok(NULL, "/");
    }
    if (name != NULL) {
        strcpy(result->endpoint_path_to_be_passed, name);
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
    return 0;
}

uint8_t virtual_fs_create_directory(char* path) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path, NULL);
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
        create_fs_entry_path_resolver(path, NULL);
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
uint8_t virtual_fs_remove_directory_entries(struct virtual_fs_directory* directory) {
    for (uint32_t i = 0; i < directory->entries_count; i++) {
        if (directory->entries[i].type == ENTRY_TYPE_DIR) {
            virtual_fs_remove_directory_entries(directory->entries[i].pointer);
            free(directory->entries[i].pointer);
        }
        if (directory->entries[i].type == ENTRY_TYPE_ENDPOINT) {
            free(directory->entries[i].pointer);
        }
    }
    free(directory->entries);
    return 0;
}
uint8_t virtual_fs_remove_directory(char* path) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path, NULL);
    serial_printf("Removing directory %s\n", result->name);
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
    if (virtual_fs_get_directory_entry(result->parent->pointer, result->name) == NULL) {
        return 1;
    }
    virtual_fs_remove_directory_entries(result->parent->pointer);
    free(result);
    free(result->parent->pointer);
    return 0;
}

// returns pointer to directory
struct virtual_fs_directory_entry*
virtual_fs_search_directory(char* path, struct virtual_fs_directory_entry* parent) {
    struct create_fs_directory_entry_path_result* result =
        create_fs_entry_path_resolver(path, parent);
    return result->parent;
}

uint8_t virtual_fs_init() {
    virtual_fs_root = malloc(sizeof(struct virtual_fs_directory_entry));
    strcpy(virtual_fs_root->name, "root");
    virtual_fs_root->type = ENTRY_TYPE_DIR;
    virtual_fs_root->pointer = malloc(sizeof(struct virtual_fs_directory));
    memset(virtual_fs_root->pointer, 0, sizeof(struct virtual_fs_directory));
}

// int virtual_fs_mkdir(const char* path, mode_t mode) {
//     struct path_result* result = virtual_fs_endpoint_path_resolver((char*)path);
//     if (result->endpoint == NULL) {
//         virtual_fs_create_directory((char*)path);
//     } else {
//         ((struct virtual_fs_endpoint*) result->endpoint->pointer)->fuse_ops.mkdir(result->endpoint_path_to_be_passed, mode);
//     }
//     return 0;
// }
// int virtual_fs_rmdir(const char* path) {
//     struct path_result* result = virtual_fs_endpoint_path_resolver((char*)path);
//     if (result->endpoint == NULL) {
//         virtual_fs_remove_directory((char*)path);
//     } else {
//         ((struct virtual_fs_endpoint*) result->endpoint->pointer)->fuse_ops.rmdir(result->endpoint_path_to_be_passed);
//     }
// }
