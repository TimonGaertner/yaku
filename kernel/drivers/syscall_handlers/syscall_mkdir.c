#include <virtual_fs/virtual_fs.h>

int64_t syscall_mkdir(const char* pathname, mode_t mode) {
    return virtual_fs_mkdir(pathname, mode);
}
