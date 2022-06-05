#include <drivers/input/input_device.h>
#include <drivers/input/ps2.h>
#include <drivers/lba/lba.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <lib/input/keyboard_handler.h>
#include <lib/input/mouse_handler.h>
#include <lib/write_to_drive.h>
#include <memory/pmm.h>
#include <multitasking/scheduler.h>
#include <multitasking/task.h>
#include <printf.h>
#include <resources/keyboard_keymap.h>
#include <stivale2.h>
#include <string.h>
#include <types.h>
#include <echfs/echfs-utils.h>
// #include <echfs/mkfs.echfs.h>
// #include <drivers/lba/lba.h>

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

void test_task() {
    serial_printf("test task\n");
}
void test_task2() {
    scheduler_sleep(1000);
    serial_printf("hello world\n");
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

    // task_add(&test_task, TASK_PRIORITY_MEDIUM, 0);
    // task_add(&test_task2, TASK_PRIORITY_MEDIUM, 0);
    lba_init();
    // lba_read_primary_controller(2, 1);
    // lba_write_primary_controller(2, 1);
    // lba_read_primary_controller(2, 1);
    uint8_t buffer[512];
    buffer[0] = 0b11111111;
    for (int i = 0; i < 256; i++) {
        buffer[i] = 0;
    }
    // buffer[0]=1;
    // lba_init();
    // lba_write_primary_controller_first_drive(0, 1, (uint8_t*)buffer);
    // buffer[0]=0;
    // lba_write_primary_controller(0, 1, &buffer[0]);

    // static struct drive_image* image;

    // image = fopen(drive_first, W);
    // serial_printf("%p\n", image);
    // fseek(image, 0, SEEK_SET);
    // fwrite(buffer, 1, 10, image);
    // lba_read_primary_controller_first_drive(0, 1, (uint8_t*)buffer);
    
    // fread(buffer, 1, 1, image);
    char* args[4] = {
        "-v",
        "",
        "ls",
        "/"
    };
    serial_printf("%lu size\n", get_drive_size(primary_controller, first_drive));
    // main(4, args);
    echfs_utils_main(4, args);
    serial_printf("buffer0: %d\n", buffer[0]);
    for (;;) {
        asm("hlt");
    }
};
