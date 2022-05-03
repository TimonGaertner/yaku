; x86_64
%macro pusha 0
    push rax
    push rcx
    push rdx
    push rbx
    ; push rsp
    push rbp
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
    pop rbp
    ; pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

; void switchTask(uint64_t *from_rsp, uint64_t *to_rsp);
global switch_task
switch_task:
    pusha ; push all registers
    mov [rdi], rsp ; save rsp into from_rsp
    mov rsp, [rsi] ; load rsp from to_rsp into rsp
    popa ; pop all registers
    ret



