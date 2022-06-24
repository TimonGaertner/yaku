#include <drivers/input/input_device.h>
#include <drivers/input/ps2.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <lib/input/keyboard_handler.h>
#include <lib/input/mouse_handler.h>
#include <memory/pmm.h>
#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <printf.h>
#include <resources/keyboard_keymap.h>
#include <stivale2.h>
#include <string.h>
#include <types.h>
#include <lib/syscall_wrapper/syscalls.h>

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
void test_task2(){
    serial_printf("test_task2\n");
}
void test_task(uint64_t test_parameter1, uint64_t test_parameter2, uint64_t test_parameter3, uint64_t test_parameter4, uint64_t test_parameter5, uint64_t test_parameter6){
    serial_printf("test_task: %d %d %d %d %d %d\n", test_parameter1, test_parameter2, test_parameter3, test_parameter4, test_parameter5, test_parameter6);
    clone(&test_task2, NULL);
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

    asm("cli");
    ps2_init();
    input_device_create_device("keyboard", "keyboard", keyboard_keymap,
                               &keyboard_handler);
    input_device_create_device("mouse", "mouse", NULL, &mouse_handler);
    asm("sti");
    task_parameters_t parameters = {.first = 1, .second = 2, .third = 3, .fourth = 4, .fifth = 5, .sixth = 6};
    task_add(test_task, &parameters, TASK_PRIORITY_HIGH, 0);
    for (;;) {
        asm("hlt");
    }
};
