
#include "ctx.h"

    .file	"ctx.S"
	.text
	.p2align 4,,15
	.globl	get_ctx
	.type	get_ctx, @function
get_ctx:
.LFB0:
	.cfi_startproc
    movq     %rax, O_RAX(%rdi)
    movq     %rbx, O_RBX(%rdi)
    movq     %rcx, O_RCX(%rdi)
    movq     %rdx, O_RDX(%rdi)
    movq     %rsi, O_RSI(%rdi)
    movq     %rdi, O_RDI(%rdi)
    movq     %rbp, O_RBP(%rdi)
    movq     %r8 , O_R8 (%rdi)
    movq     %r9 , O_R9 (%rdi)
    movq     %r10, O_R10(%rdi)
    movq     %r11, O_R11(%rdi)
    movq     %r12, O_R12(%rdi)
    movq     %r13, O_R13(%rdi)
    movq     %r14, O_R14(%rdi)
    movq     %r15, O_R15(%rdi)
    movq     %rsp, %rcx
    movq     %rcx, O_RIP(%rdi)
    leaq     8(%rsp), %rcx
    movq     %rcx, O_RSP(%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	get_ctx, .-get_ctx
	.p2align 4,,15
	.globl	set_ctx
	.type	set_ctx, @function
set_ctx:
.LFB1:
	.cfi_startproc
    movq     O_RSP(%rdi), %rsp
    movq     O_RAX(%rdi), %rax
    movq     O_RBX(%rdi), %rbx
    movq     O_RDX(%rdi), %rdx
    movq     O_RSI(%rdi), %rsi
    movq     O_RBP(%rdi), %rbp
    movq     O_R8 (%rdi), %r8 
    movq     O_R9 (%rdi), %r9 
    movq     O_R10(%rdi), %r10
    movq     O_R11(%rdi), %r11
    movq     O_R12(%rdi), %r12
    movq     O_R13(%rdi), %r13
    movq     O_R14(%rdi), %r14
    movq     O_R15(%rdi), %r15
    movq     O_RIP(%rdi), %rcx
    pushq    %rcx
    movq     O_RCX(%rdi), %rcx
    movq     O_RDI(%rdi), %rdi
	ret
	.cfi_endproc
.LFE1:
	.size	set_ctx, .-set_ctx
	.p2align 4,,15
	.globl	swap_ctx
	.type	swap_ctx, @function
swap_ctx:
.LFB2:
	.cfi_startproc
    movq     %rax, O_RAX(%rdi)
    movq     %rbx, O_RBX(%rdi)
    movq     %rcx, O_RCX(%rdi)
    movq     %rdx, O_RDX(%rdi)
    movq     %rsi, O_RSI(%rdi)
    movq     %rdi, O_RDI(%rdi)
    movq     %rbp, O_RBP(%rdi)
    movq     %r8 , O_R8 (%rdi)
    movq     %r9 , O_R9 (%rdi)
    movq     %r10, O_R10(%rdi)
    movq     %r11, O_R11(%rdi)
    movq     %r12, O_R12(%rdi)
    movq     %r13, O_R13(%rdi)
    movq     %r14, O_R14(%rdi)
    movq     %r15, O_R15(%rdi)
    movq     %rsp, %rcx
    movq     %rcx, O_RIP(%rdi)
    leaq     8(%rsp), %rcx
    movq     %rcx, O_RSP(%rdi)

    movq     O_RSP(%rsi), %rsp
    movq     O_RAX(%rsi), %rax
    movq     O_RBX(%rsi), %rbx
    movq     O_RDX(%rsi), %rdx
    movq     O_RBP(%rsi), %rbp
    movq     O_RDI(%rsi), %rdi
    movq     O_R8 (%rsi), %r8 
    movq     O_R9 (%rsi), %r9 
    movq     O_R10(%rsi), %r10
    movq     O_R11(%rsi), %r11
    movq     O_R12(%rsi), %r12
    movq     O_R13(%rsi), %r13
    movq     O_R14(%rsi), %r14
    movq     O_R15(%rsi), %r15
    movq     O_RIP(%rsi), %rcx
    pushq    %rcx
    movq     O_RCX(%rsi), %rcx
    movq     O_RSI(%rsi), %rsi
	ret
	.cfi_endproc
.LFE2:
	.size	swap_ctx, .-swap_ctx
	.ident	"GCC: (Ubuntu/Linaro 4.7.3-1ubuntu1) 4.7.3"
	.section	.note.GNU-stack,"",@progbits
