#include <virtual_fs/virtual_fs.h>

int64_t syscall_unlink(const char* pathname) {
    return virtual_fs_unlink(pathname);
}
