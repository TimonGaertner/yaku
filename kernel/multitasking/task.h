#pragma once
// #include <drivers/pit.h>
// #include <multitasking/scheduler.h>
#include <types.h>
#define MAX_TASKS 100
#define TASK_STACK_SIZE 1024
// bool task_exists;


enum task_priority { TASK_PRIORITY_LOW, TASK_PRIORITY_MEDIUM, TASK_PRIORITY_HIGH, TASK_PRIORITY_VERY_HIGH };
typedef struct task {
    uint64_t *rsp;
    uint64_t stack[TASK_STACK_SIZE];
    void* function;
    bool filled;
    uint32_t sleep_till;
    uint32_t pid;
    uint32_t parent_pid;
    enum task_priority priority;
    struct task* next;
} task_t;

// task_t task_list[MAX_TASKS];
// task_t* current_task;

// switch to task
extern switch_task(uint64_t *from, uint64_t *to);
// extern switch_task_and_jump(task_t* task);
task_t* task_create(void* function);
void print();