#pragma once
#include <types.h>
#include <multitasking/task.h>
// enum syscalls {
//     SYS_create_task,
//     SYS_sleep,
//     SYS_pause,
//     SYS_kill,
//     SYS_get_pid,
//     SYS_get_ppid,
//     SYS_get_time,
//     SYS_get_ticks,
//     SYS_get_window_buffer
// };

struct SYS_create_task_parameters {
    void* function;
    task_parameters_t parameters;
};

void isr_syscall(uint64_t ptr);
