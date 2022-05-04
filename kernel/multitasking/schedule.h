#pragma once
#include <multitasking/task.h>
#include <types.h>
#include <drivers/pit.h>

#define MAX_SCHEDULED_TASKS MAX_TASKS * 4
#define SWITCH_INTERVAL_MS 100

static struct task *current_task;
void schedule_tasks();
void schedule_nothing();
void scheduler_task();
void schedule_set_task_terminated();
static void* schedule_switch_task;
void schedule_init(void* kernel_function);
