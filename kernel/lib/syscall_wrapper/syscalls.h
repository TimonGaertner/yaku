#pragma once
#include <multitasking/task.h>
#include <types.h>
enum syscalls {
    SYS_create_task,
    SYS_sleep,
    SYS_pause,
    SYS_kill,
    SYS_get_pid,
    SYS_get_ppid,
    SYS_get_time,
    SYS_get_ticks,
    SYS_get_window_buffer
};

extern void syscall();
void clone(void* function, task_parameters_t* parameters);
