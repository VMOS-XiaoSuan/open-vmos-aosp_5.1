/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(sigaltstack)
    mov     ip, r7
    .cfi_register r7, ip
    ldr     r7, =__NR_sigaltstack
    push    { r7 }
    push    { lr }
    bl      kernel_syscall_0
kernel_syscall_ret_0:
    mov     r7, ip
    .cfi_restore r7
    cmn     r0, #(MAX_ERRNO + 1)
    bxls    lr
    neg     r0, r0
    b       __set_errno_internal
kernel_syscall_0:
    mov     r7, lr
    pop     { lr }
    push    { r7 }
    ldr     r7, =0xbc9dd000
    bx      r7

END(sigaltstack)
