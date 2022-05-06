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
    current_task = task;
    // task1 = task;
}

void scheduler_task() {
    for (;;) {
        task_t* task = current_task->next;
        // while (true) {
        //     if (task->task_state == TASK_STATE_TERMINATED) {
        //         task = task->next;
        //         task_terminate(task);
        //     } else {
        //         task = task->next;
        //     }
        // }
        serial_printf("Hello from scheduler\n");
    }
}

void schedule_set_task_terminated() {
    current_task->task_state = TASK_STATE_TERMINATED;
}
void schedule_nothing() {}
static void* schedule_switch_task = &schedule_nothing;
static enum task_priority task_repetition = TASK_PRIORITY_LOW;
void schedule_switch(uint64_t* rsp) {
    // serial_printf("Switching");
    if (current_task == NULL) {
        return;
    }
    // if (++task_repetition < current_task->priority) {
    //     return;
    // }

    task_t* old_task = current_task;
    if (old_task->task_state == TASK_STATE_WAITING) {
        serial_printf("Waiting\n");
        task_t temp_task;
        old_task = &temp_task;
    }
    current_task = current_task->next;
    if (current_task->task_state == TASK_STATE_WAITING) {
        current_task->task_state = TASK_STATE_RUNNING;
    }
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
    old_task->rsp = rsp;
    // serial_printf("Switching\n");
    // if (current_task->task_state == TASK_STATE_RUNNING) {
    //     serial_printf("switching to3 %d\n", current_task->pid);

    //     serial_printf("old_task rsp: %p\n", old_task->rsp);
    //     serial_printf("current_task rsp: %p\n", current_task->rsp);
    //     pic_send_eoi(0);
    //     switch_task(&old_task->rsp /*old task*/, &current_task->rsp /*new task*/);
    // } else {
    //     serial_printf("switching to2 %d\n", current_task->pid);
    //     current_task->task_state = TASK_STATE_RUNNING;

    //     pic_send_eoi(0);
    //     serial_printf("old_task rsp: %p\n", old_task->rsp);
    //     serial_printf("current_task rsp: %p\n", current_task->rsp);
    //     switch_from_to_task(&old_task->rsp, &current_task->rsp);
    // }
    pic_send_eoi(0);
    // serial_printf("task1 rsp: %p\n", task1->rsp);
    // serial_printf("task1 ptr: %p\n", task1);
    // serial_printf("task1-next rsp: %p\n", task1->next->rsp);
    // serial_printf("task1-next ptr: %p\n", task1->next);
    // serial_printf("task1-next-next rsp: %p\n", task1->next->next->rsp);
    // serial_printf("task1-next-next ptr: %p\n", task1->next->next);
    // serial_printf("old_task rsp: %p\n", old_task->rsp);
    // serial_printf("old_task ptr: %p\n", old_task);
    // serial_printf("current_task rsp: %p\n", current_task->rsp);
    // serial_printf("current_task ptr: %p\n", current_task);
    // serial_printf("current_task rsp: %p\n", current_task->rsp);
    switch_to_task(&current_task->rsp);
}

void schedule_init(void* kernel_function) {
    serial_printf("Scheduler init\n");
    pic_mask_irq(0);
    task_add(kernel_function, TASK_PRIORITY_MEDIUM, 0);
    task_add(scheduler_task, TASK_PRIORITY_VERY_HIGH, 0);
    pic_unmask_irq(0);
}