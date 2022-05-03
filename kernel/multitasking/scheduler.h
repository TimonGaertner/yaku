#pragma once
#include <multitasking/task.h>
#include <types.h>

#define MAX_SCHEDULED_TASKS MAX_TASKS * 4
#define SWITCH_INTERVAL_MS 100



static uint32_t scheduler_list_pointer = 0;

void schedule_tasks();
void exit();

