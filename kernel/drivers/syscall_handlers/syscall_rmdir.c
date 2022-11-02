#include <virtual_fs/virtual_fs.h>

int64_t syscall_rmdir(const char* pathname) {
    return virtual_fs_rmdir(pathname);
}
