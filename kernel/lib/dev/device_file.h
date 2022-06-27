#include <lib/fuse.h>
#include <types.h>

struct dev_file {
    char name[100];
    struct fuse_operations fuse_ops;
};
enum dev_file_type { DEV_FOLDER, DEV_FILE };
struct dev_file_pointer {
    enum dev_file_type type;
    uint64_t* ptr;
};
struct dev_dir {
    char name[100];
    uint64_t pointer_count;
    struct dev_file_pointer* pointers;
};

void dev_dir_add_folder(char* path);
void dev_dir_create_file(char* path, struct fuse_operations* fuse_ops);
void dev_dir_init();
void dev_dir_get_fuse_ops(char* path, struct fuse_operations* fuse_ops);
void dev_dir_readdir(char* path, void (*callback)(char* name, enum dev_file_type type));
