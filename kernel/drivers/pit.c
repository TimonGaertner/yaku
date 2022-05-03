#include "pit.h"

#include <io.h>
#include <types.h>
#include <multitasking/scheduler.h>

static uint32_t tick = 0;

void pit_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    io_outb(0x43, 0x36);

    uint8_t lower = (uint8_t)(divisor & 0xFF);
    uint8_t upper = (uint8_t)((divisor >> 8) & 0xFF);

    io_outb(0x40, lower);
    io_outb(0x40, upper);
}
enum task_priority task_repetition = TASK_PRIORITY_LOW;
void pit_tick_increment(void) {
    tick++;
    // if (++task_repetition < current_task->priority) {
    //     return;
    // }
    // while (current_task->sleep_till > tick) {
    //     current_task = current_task->next;
    // }
    // if (current_task->sleep_till > 0) {
    //     current_task->sleep_till = 0;
    // }
    // task_t *old_task = &current_task;
    // current_task = current_task->next;
    // switch_task(&old_task->regs /*old task*/, &current_task->regs /*new task*/);
}

uint32_t pit_tick_get(void) {
    return tick;
}

uint32_t pit_read(void) {
    uint32_t count = 0;

    asm("cli");

    io_outb(0x43, 0b0000000);

    count = io_inb(0x40);
    count |= io_inb(0x40) << 8;

    asm("sti");

    return count;
}
