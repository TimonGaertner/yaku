typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
} registers_t;



typedef struct task{
    registers_t regs;
} task_t;