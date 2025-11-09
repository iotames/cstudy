section .data                           ; 数据段，定义要输出的字符串
    hello db 'Hello, World!', 0xA      ; 字符串内容，0xA是换行符
    hello_len equ $ - hello             ; 计算字符串长度

section .text                           ; 代码段
    global _start                       ; 告知汇编器程序入口点

_start:                                 ; 程序入口标签
    ; 执行系统调用 write(1, hello, hello_len)
    mov eax, 4              ; 系统调用号，4代表 sys_write
    mov ebx, 1              ; 文件描述符，1代表标准输出(stdout)
    mov ecx, hello           ; 要输出的字符串的地址
    mov edx, hello_len       ; 要输出的字符串的长度
    int 0x80                 ; 发起软中断，调用内核功能

    ; 执行系统调用 exit(0) 正常退出程序
    mov eax, 1              ; 系统调用号，1代表 sys_exit
    mov ebx, 0              ; 退出状态码，0表示正常
    int 0x80                 ; 发起软中断