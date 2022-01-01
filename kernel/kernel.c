
#include <printf.h>

#include <drivers/input/input_device.h>
#include <keyboard_keymap.h>
#include <keyboard_stringmap.h>
#include <drivers/input/ps2.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/vga_text.h>
#include <interrupts/idt.h>
#include <interrupts/isr.h>
#include <interrupts/pic.h>
#include <input/read_input.h>
#include <multiboot.h>
#include <types.h>
#include <lib/string.h>
#include <lib/input/input_to_text.h>

#include <input/read_input.h>

void input_init() {
    ps2_init();
    input_device_create_device("keyboard", "keyboard", keyboard_keymap);
    input_to_text_init();

}

void kernel_main(multiboot_info_t* mb_info) {
    serial_init();

    vga_text_init(mb_info->framebuffer_width, mb_info->framebuffer_height);
    vga_text_set_style(VGA_TEXT_COLOR_RED, VGA_TEXT_COLOR_BLACK);

    pic_init();
    idt_init();
    pit_init(1000);

    input_init();
    read_input_listener listener;
    read_input_init_listener(&listener);
    timer_sleep_ticks(1000);
    uint8_t buffer[256];
    read_input_get_keystrokes(&listener, buffer);
    serial_printf("lol: %s\n", buffer[0]);
    char* str = "hello world";
    serial_printf("Hello, %s!\n", strcat_inbetween(str, " ", 12));

    for (;;) {
        asm("hlt");
    }
};