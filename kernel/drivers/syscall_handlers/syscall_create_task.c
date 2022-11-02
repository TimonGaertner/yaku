#include "syscall.h"
#include <multitasking/scheduler.h>
#include <multitasking/task.h>

uint64_t syscall_create_task(void* function, task_parameters_t* task_parameters) {
    task_t* task = task_add(function, &task_parameters, TASK_PRIORITY_LOW, 0);
    if (task == NULL) {
        return -1;
    }
    return task->pid;
}
