bits 64
default rel

section .data
    msg db 'Hello, World from ASM!', 0

section .text
    global main
    extern printf
    extern exit ; 它告诉汇编器 exit是一个外部函数，其实现不在当前文件中，需要在链接时从C运行时库中解析。

main:
    sub rsp, 40        ; 分配40字节栈空间（32字节影子空间+8字节对齐）
    
    ; 调用printf
    lea rcx, [msg]
    call printf
    
    ; 直接退出，不恢复栈
    mov ecx, 0
    call exit
    
    ; 程序不会执行到这里
    add rsp, 40
    ret

; 声明exit函数
extern exit