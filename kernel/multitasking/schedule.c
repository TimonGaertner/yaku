#include "schedule.h"
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <interrupts/pic.h>
static task_t* current_task;
static uint64_t task_count = 0;
// Schedule Tasks
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
                task = task->next;
                task_terminate(task);
            } else {
                task = task->next;
            }
        }
    }
}

void schedule_set_task_terminated() {
    current_task->task_state = TASK_STATE_TERMINATED;
}
void schedule_nothing() {}
static void* schedule_switch_task = &schedule_nothing;
static enum task_priority task_repetition = TASK_PRIORITY_LOW;
void schedule_switch() {

    serial_printf("Switching");
    if (current_task == NULL) {
        return;
    }
    if (++task_repetition < current_task->priority) {
        return;
    }

    task_t* old_task = &current_task;
    current_task = current_task->next;
    // while (current_task->task_state == TASK_STATE_TERMINATED) {
    //     current_task = current_task->next;
    // }
    // if (current_task->task_state == TASK_STATE_SLEEP) {
    //     while (current_task->sleep_till > pit_get_ticks()) {
    //         current_task = current_task->next;
    //     }
    //     if (current_task->sleep_till > 0) {
    //         current_task->sleep_till = 0;
    //     }
    // }

    serial_printf("Switching\n");
    if (current_task->task_state == TASK_STATE_RUNNING) {
        switch_task(&old_task->rsp /*old task*/, &current_task->rsp /*new task*/);
    } else if (current_task->task_state == TASK_STATE_WAITING) {
        switch_to_task(&current_task->rsp);
    }
}

void schedule_init(void* kernel_function) {
    serial_printf("Scheduler init\n");
    task_add(kernel_function, TASK_PRIORITY_MEDIUM, 0);
    task_add(scheduler_task, TASK_PRIORITY_VERY_HIGH, 0);
    schedule_switch_task = &schedule_switch;
}