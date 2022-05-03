#include <drivers/input/input_device.h>
#include <drivers/input/ps2.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <memory/pmm.h>
#include <multitasking/task.h>
#include <printf.h>
#include <resources/keyboard_keymap.h>
#include <stivale2.h>
#include <string.h>
#include <types.h>
// https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
// https://github.com/SamuelYvon/Multitasking
//  https://github.com/jagatsastry/deepos/blob/d73f4c4260fdfbf61a2e40c8d467241e9836fcdb/sys/scheduler.c
// https://wiki.osdev.org/Cooperative_Multitasking
extern int enable_sse();

static uint8_t stack[8192];

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag =
        {
            .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
            .next = 0,
        },
    .flags = 0,
};

static stivale2_header_tag_framebuffer_t framebuffer_hdr_tag = {
    .tag =
        {
            .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
            .next = (uintptr_t)&terminal_hdr_tag,
        },
    // pick best automatically
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0,
};

__attribute__((section(".stivale2hdr"), used)) static stivale2_header_t stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uintptr_t)&framebuffer_hdr_tag,
};

void* stivale2_get_tag(stivale2_struct_t* stivale2_struct, uint64_t id) {
    stivale2_tag_t* current_tag = (void*)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) {
            return NULL;
        }
        if (current_tag->identifier == id) {
            return current_tag;
        }
        current_tag = (void*)current_tag->next;
    }
}

void task_test1() {
    serial_printf("hallo\n");
    serial_printf("task1\n");
}
void start(stivale2_struct_t* stivale2_struct) {

    enable_sse();
    serial_init();
    pic_init();
    idt_init();
    pit_init(1000);

    stivale2_struct_tag_memmap_t* memory_map;
    memory_map = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    pmm_init(memory_map);
    ps2_init();
    input_device_create_device("keyboard", "keyboard", keyboard_keymap);

    // asm volatile("movq %%cr3, %%rax; movq %%rax, %0;":"=m"(task1.regs.cr3)::"%rax");
    // asm volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0;
    // popfq;":"=m"(task1.regs.eflags)::"%rax");

    task_t* task1 = task_create(&task_test1);
    uint64_t* rsp = &(task1->stack[TASK_STACK_SIZE-1]);
    serial_printf("%p\n", task1->stack[TASK_STACK_SIZE-1]);
    serial_printf("%p\n", &task_test1);
    switch_task(&rsp, &task1->rsp);
    serial_printf("%p\n", rsp);

    for (;;) {
        asm("hlt");
    }
};
