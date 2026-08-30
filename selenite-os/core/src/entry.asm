.global _start
.extern kernel_main

.section .text

_start:
    cli
    call kernel_main

.hang:
    hlt
    jmp .hang

.section .note.GNU-stack,"",@progbits
