#include "syscalls.h"
#include <drivers/serial.h>
void clone(void* function, task_parameters_t* parameters) {
    serial_printf("clone\n");
    syscall(SYS_get_pid, function, parameters);
}
