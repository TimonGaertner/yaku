#include "syscall.h"
#include <drivers/serial.h>
#include <lib/syscall_wrapper/syscalls.h>


void syscall_create_task(void* function, task_parameters_t* task_parameters){
    task_t* task = task_add(function, &task_parameters, TASK_PRIORITY_LOW, 0);
}

void isr_syscall(uint64_t rid, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    enum syscalls syscall = (enum syscalls) rid;
    serial_printf("Syscall: %d\n", rid);
    switch (syscall)
    {
    case SYS_create_task:
        syscall_create_task((void*) rsi, (task_parameters_t*) rdx);
        break;
    
    default:
        break;
    }
    return;
}
