#include "schedule.h"
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <interrupts/pic.h>
static task_t* current_task;
static uint64_t task_count = 0;
// Schedule Tasks
// static task_t* task1;
void schedule_task(task_t* task) {
    if (current_task == NULL) {
        current_task = task;
        current_task->next = task;
    } else {
        task->next = current_task->next;
        current_task->next = task;
    }
}

void scheduler_task() {
    for (;;) {
        task_t* task = current_task->next;
        while (true) {
            if (task->task_state == TASK_STATE_TERMINATED) {
                serial_printf("Task %d found\n", task->pid);
                task_terminate(task);
                task = task->next;
            } else {
                task = task->next;
            }
        }
    }
}

void schedule_set_task_terminated() {
    current_task->task_state = TASK_STATE_TERMINATED;
}
static enum task_priority task_repetition = TASK_PRIORITY_LOW;
void schedule_switch(uint64_t* rsp) {
    // serial_printf("Switching");
    if (current_task == NULL || task_repetition++ < current_task->priority) {
        return;
    }
    task_repetition = TASK_PRIORITY_LOW;
    task_t* old_task = current_task;
    current_task = current_task->next;
    if (current_task->task_state == TASK_STATE_WAITING) {
        current_task->task_state = TASK_STATE_RUNNING;
    }

    if (old_task->task_state != TASK_STATE_WAITING) {
        old_task->rsp = rsp;
    }
    if (current_task->task_state == TASK_STATE_TERMINATED) {
        current_task = current_task->next;
    }
    pic_send_eoi(0);
    switch_to_task(&current_task->rsp);
}

void schedule_init(void* kernel_function) {
    pic_mask_irq(0);
    task_add(kernel_function, TASK_PRIORITY_LOW, 0);
    task_add(&scheduler_task, TASK_PRIORITY_LOW, 0);
    pic_unmask_irq(0);
}