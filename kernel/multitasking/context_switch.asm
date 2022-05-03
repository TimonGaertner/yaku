; x86_64
%macro pusha 0
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro 

%macro popa 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
%endmacro
extern print
; void switchTask(uint64_t *from_rsp, uint64_t *to_rsp);
global switch_task
switch_task:
    pusha ; push all registers
    mov [rdi], rsp ; save rsp into from_rsp
    mov rsp, [rsi] ; load rsp from to_rsp into rsp
    popa ; pop all registers
    ret



