#include "device_file.h"
#include <lib/fuse.h>
#include <memory/pmm.h>
#include <string.h>
#include <types.h>
#include <drivers/serial.h>
static struct dev_dir* dev_dir_root = NULL;
struct path_result {
    struct dev_dir* parent_dir;
    char name[100];
};
uint8_t path_resolver(char* path, struct path_result* result) {
    uint16_t num_of_path_dirs = 0;
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == '/') {
            num_of_path_dirs++;
        }
    }
    char* token = strtok(path, "/");
    struct dev_dir* dir = dev_dir_root;
    // serial_printf("path: %s\n", token);
    while (token != NULL && num_of_path_dirs > 0) {
        serial_printf("token: %s\n", token);
        for (uint64_t i = 0; i < dir->pointer_count; i++) {
            if (strcmp(((struct dev_dir*)dir->pointers[i].ptr)->name, token) == 0) {
                dir = (struct dev_dir*)dir->pointers[i].ptr;
                break;
            }
        }
        if (strcmp(dir->name, token) != 0) {
            return 1;
        }
        num_of_path_dirs--;
        token = strtok(NULL, "/");
    }
    result->parent_dir = dir;
    result->name[0]='\0';
    if (token != NULL) {
        strcpy(result->name, token);
    }

    serial_printf("name3: %s\n", result->name);
    return 0;
}
void dev_dir_add_folder(char* path) {
    struct path_result result;
    if (path_resolver(path, &result)!=0){
        return 1;
    }
    if (result.name[0] == '\0') {
        return 1;
    }
    struct dev_dir* dir = result.parent_dir;
    struct dev_dir* new_dir = (struct dev_dir*)malloc(sizeof(struct dev_dir));
    new_dir->pointer_count = 0;
    strcpy(new_dir->name, result.name);
    dir->pointer_count++;
    dir->pointers = (struct dev_file_pointer*)realloc(
        dir->pointers, sizeof(struct dev_file_pointer) * dir->pointer_count);
    dir->pointers[dir->pointer_count-1].ptr = new_dir;
    dir->pointers[dir->pointer_count-1].type = DEV_FOLDER;
}
void dev_dir_create_file(char* path, struct fuse_operations* fuse_ops) {
    struct path_result result;
    if (path_resolver(path, &result)!=0){
        return 1;
    }
    if (result.name[0] == '\0') {
        serial_printf("name is empty\n");
        return 1;
    }
    // serial_printf("name %s\n", result.name);
    struct dev_dir* dir = result.parent_dir;
    struct dev_file* file = (struct dev_file*)malloc(sizeof(struct dev_file));
    strcpy(file->name, result.name);
    // serial_printf("name %s\n", file->name);
    dir->pointer_count++;
    dir->pointers = (struct dev_file_pointer*)realloc(
    dir->pointers, sizeof(struct dev_file_pointer) * dir->pointer_count);
    dir->pointers[dir->pointer_count-1].ptr = file;
    dir->pointers[dir->pointer_count-1].type = DEV_FILE;
    // serial_printf("name %s\n", ((struct dev_file*)dir->pointers[dir->pointer_count-1].ptr)->name);
    memcpy(&file->fuse_ops, fuse_ops, sizeof(struct fuse_operations));
    // serial_printf("ptr1 %p\n", dir->pointers[dir->pointer_count-1].ptr);
}
void dev_dir_readdir(char* path, void (*callback)(char* name, enum dev_file_type type)) {
    struct path_result result;
    if (path[strlen(path)-1] != '/') {
        strcat(path, "/");
    }
    if (path_resolver(path, &result)!=0){
        return 1;
    }
    struct dev_dir* dir = result.parent_dir;
    serial_printf("dir: %s\n", dir->name);
    serial_printf("pointer_count: %d\n", dir->pointer_count);
    serial_printf("ptr2 %p\n", dir->pointers[0].ptr);
    for (uint64_t i = 0; i < dir->pointer_count; i++) {
        serial_printf("name: %s\n", ((struct dev_file*)dir->pointers[i].ptr)->name);
        callback(((struct dev_dir*)dir->pointers[i].ptr)->name, dir->pointers[i].type);
    }
}
void dev_dir_get_fuse_ops(char* path, struct fuse_operations* fuse_ops) {
    struct path_result result;
    path_resolver(path, &result);
    struct dev_dir* dir = result.parent_dir;
    for (uint64_t i = 0; i < dir->pointer_count; i++) {
        if (strcmp(((struct dev_dir*)dir->pointers[i].ptr)->name, result.name) == 0) {
            memcpy(fuse_ops, &((struct dev_file*)dir->pointers[i].ptr)->fuse_ops, sizeof(struct fuse_operations));
            return;
        }
    }
}
void dev_dir_init() {
    dev_dir_root = malloc(sizeof(struct dev_dir));
    strcpy(dev_dir_root->name, "dev");
    dev_dir_root->pointer_count = 0;
    dev_dir_root->pointers = NULL;
}
