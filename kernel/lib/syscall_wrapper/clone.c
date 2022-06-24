#include "syscalls.h"
#include "syscall.c"
void clone(void* function, task_parameters_t parameters) {
    syscall(SYS_create_task, function, parameters);
}
