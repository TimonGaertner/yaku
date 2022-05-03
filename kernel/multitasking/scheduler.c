#include "scheduler.h"




void schedule_empty_function() {
    for (;;) {
        asm("hlt");
    }
}

static task_t scheduler_empty_task = {
    .function = &schedule_empty_function,
    .priority = 0,
    .parent_pid = NULL,
    .filled = true,
};


// Schedule Tasks
void scheduler_tasks() {
    if (!task_exists) {
        current_task = &scheduler_empty_task;
        return;
    }
    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].filled) {
            if (task_list[i+1%MAX_TASKS].filled) {
                task_list[i].next = &task_list[i+1%MAX_TASKS];
            }
        }
    }

}

void exit() {
    task_remove(current_task);
}