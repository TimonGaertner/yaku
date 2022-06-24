#include "syscall.h"
#include <drivers/serial.h>

void isr_syscall(uint64_t* ptr){
    enum syscalls syscall = (enum syscalls) ptr;
    serial_printf("Syscall: %d\n", syscall);
    return;
}
