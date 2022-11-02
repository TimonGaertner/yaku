#include <lib/stat.h>
#include <virtual_fs/FILE.h>
#include <virtual_fs/virtual_fs.h>

int64_t syscall_mknod(const char* pathname, mode_t mode, dev_t dev) {
    return virtual_fs_mknod(pathname, mode, dev);
}
