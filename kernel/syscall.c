#include "syscall.h"
#include <drivers/serial.h>
#include <lib/syscall_wrapper/syscalls.h>
void isr_syscall(uint64_t ptr){
    enum syscalls syscallf = (enum syscalls) ptr;
    serial_printf("Syscall: %d\n", ptr);
    return;
}
