#include "task.h"
#include <drivers/serial.h>
// task_exists = false;
// // The task list
// static task_t task_list[MAX_TASKS];

// static task_t *current_task;

// // Initialize the tasking system
// void task_init()
// {
//     // Initialize the task list
//     for (int i = 0; i < MAX_TASKS; i++)
//     {
//         task_list[i].filled = false;
//     }
// }

// // Add a task to the task list
// void task_add(void* function, enum task_priority priority, uint32_t parent_pid)
// {

//     if (!task_exists)
//     {
//         task_exists = true;
//     }
//     if (function == NULL)
//     {
//         return;
//     }
//     task_t task = {
//         .function = function,
//         .priority = priority,
//         .parent_pid = parent_pid,
//         .filled = true,
//     };
//     // Find an empty slot
//     for (int i = 0; i < MAX_TASKS; i++)
//     {
//         if (!task_list[i].filled)
//         {

//             task.pid = i;
//             task.regs.rip = (uint64_t)function;
//             task_list[i] = task;
//             scheduler_tasks();

//             return;
//         }
//     }
// }

// // Remove a task from the task list
// void task_remove(task_t *task)
// {
//     uint32_t pid = task->pid;
//     // Find the task
//     for (int i = 0; i < MAX_TASKS; i++)
//     {
//         if (task_list[i].filled && (task_list[i].pid == task->pid ||
//         task_list[i].parent_pid == task->parent_pid))
//         {
//             // Mark the slot as empty
//             task_list[i].filled = false;

//             // Exit the loop
//             break;
//         }
//     }
//     scheduler_tasks();
// }

// // adds sleeping to task
// void task_sleep(task_t *task, uint32_t ticks)
// {
//     task->sleep_till = pit_tick_get()+ticks;
// }
// void task_switch(task_t *task)
// {
//     // Save the current task context
//     task_save_context(current_task);

//     // Restore the task context
//     task_restore_context(task);

//     // Set the current task
//     current_task = task;
// }

// // save task context
// void task_save_context(task_t *task)
// {
//     // Save the task context
//     asm volatile("movq %%rax, %0" : "=m"(task->regs.rax));
//     asm volatile("movq %%rbx, %0" : "=m"(task->regs.rbx));
//     asm volatile("movq %%rcx, %0" : "=m"(task->regs.rcx));
//     asm volatile("movq %%rdx, %0" : "=m"(task->regs.rdx));
//     asm volatile("movq %%rdi, %0" : "=m"(task->regs.rdi));
//     asm volatile("movq %%rsi, %0" : "=m"(task->regs.rsi));
//     asm volatile("movq %%rbp, %0" : "=m"(task->regs.rbp));
//     asm volatile("movq %%r8, %0" : "=m"(task->regs.r8));
//     asm volatile("movq %%r9, %0" : "=m"(task->regs.r9));
//     asm volatile("movq %%r10, %0" : "=m"(task->regs.r10));
//     asm volatile("movq %%r11, %0" : "=m"(task->regs.r11));
//     asm volatile("movq %%r12, %0" : "=m"(task->regs.r12));
//     asm volatile("movq %%r13, %0" : "=m"(task->regs.r13));
//     asm volatile("movq %%r14, %0" : "=m"(task->regs.r14));
//     asm volatile("movq %%r15, %0" : "=m"(task->regs.r15));
// }

// // restore task context
// void task_restore_context(task_t *task)
// {
//     // Restore the task context
//     asm volatile("movq %0, %%rax" :: "m"(task->regs.rax));
//     asm volatile("movq %0, %%rbx" :: "m"(task->regs.rbx));
//     asm volatile("movq %0, %%rcx" :: "m"(task->regs.rcx));
//     asm volatile("movq %0, %%rdx" :: "m"(task->regs.rdx));
//     asm volatile("movq %0, %%rdi" :: "m"(task->regs.rdi));
//     asm volatile("movq %0, %%rsi" :: "m"(task->regs.rsi));
//     asm volatile("movq %0, %%rbp" :: "m"(task->regs.rbp));
//     asm volatile("movq %0, %%r8" :: "m"(task->regs.r8));
//     asm volatile("movq %0, %%r9" :: "m"(task->regs.r9));
//     asm volatile("movq %0, %%r10" :: "m"(task->regs.r10));
//     asm volatile("movq %0, %%r11" :: "m"(task->regs.r11));
//     asm volatile("movq %0, %%r12" :: "m"(task->regs.r12));
//     asm volatile("movq %0, %%r13" :: "m"(task->regs.r13));
//     asm volatile("movq %0, %%r14" :: "m"(task->regs.r14));
//     asm volatile("movq %0, %%r15" :: "m"(task->regs.r15));
// }

void memset(void* dest, uint8_t val, size_t len) {
    unsigned char* ptr = dest;
    while (len-- > 0)
        *ptr++ = val;
    return dest;
}

task_t* task_create(void* function) {
    task_t* new_task = (task_t*)malloc(sizeof(task_t));
    memset(new_task->stack, 0, TASK_STACK_SIZE);
    new_task->rsp = &(new_task->stack[TASK_STACK_SIZE - 16]); // 15 regs for poping in task_switch, 1 for return address
    new_task->stack[TASK_STACK_SIZE-1] = function; // return address
    new_task->stack[TASK_STACK_SIZE-2] = &(new_task->stack[TASK_STACK_SIZE-1]); // rbp
    return new_task;
}
void print(){
    serial_printf("test");
}