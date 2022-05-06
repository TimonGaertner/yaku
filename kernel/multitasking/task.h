#pragma once
// #include <drivers/pit.h>
// #include <multitasking/scheduler.h>
#include <types.h>
#include <multitasking/schedule.h>
#include <interrupts/pic.h>
#define TASK_STACK_SIZE 1024


enum task_state {
    TASK_STATE_RUNNING,
    TASK_STATE_SLEEP,
    TASK_STATE_TERMINATED,
    TASK_STATE_WAITING,
};
enum task_priority { TASK_PRIORITY_LOW, TASK_PRIORITY_MEDIUM, TASK_PRIORITY_HIGH, TASK_PRIORITY_VERY_HIGH };
typedef struct task {
    uint64_t *rsp;
    uint64_t stack[TASK_STACK_SIZE];
    uint32_t sleep_till;
    uint32_t pid;
    uint32_t parent_pid;
    enum task_state task_state;
    enum task_priority priority;
    struct task* next;
} task_t;

// task_t task_list[MAX_TASKS];
// task_t* current_task;

// switch to task
extern switch_task(uint64_t *from, uint64_t *to);
void task_add(void* function, enum task_priority priority, uint32_t parent_pid);
task_t* task_create(void* function);
void task_exit();
void task_terminate(task_t *task);
task_t* task_get_ptr_by_pid(uint32_t pid);
task_t* task_get_ptr_by_parent_pid(uint32_t pid);
void task_kill(uint32_t pid);
extern switch_to_task(uint64_t *to);
extern switch_from_to_task(uint64_t *from, uint64_t *to);