
#include "ctx.h"

	.text
	.p2align 4,,15
	.globl	get_ctx
	.type	get_ctx, @function
get_ctx:
    movq     %rbx, O_RBX(%rdi)
    movq     %rbp, O_RBP(%rdi)
    movq     %r12, O_R12(%rdi)
    movq     %r13, O_R13(%rdi)
    movq     %r14, O_R14(%rdi)
    movq     %r15, O_R15(%rdi)

    movq     %rdi, O_RDI(%rdi)
    movq     %rsi, O_RSI(%rdi)
    movq     %rdx, O_RDX(%rdi)
    movq     %rcx, O_RCX(%rdi)
    movq     %r8 , O_R8 (%rdi)
    movq     %r9 , O_R9 (%rdi)

    movq     (%rsp), %rcx
    movq     %rcx, O_RIP(%rdi)
    leaq     8(%rsp), %rcx
    movq     %rcx, O_RSP(%rdi)

	xorl	%eax, %eax
	ret

	.p2align 4,,15
	.globl	set_ctx
	.type	set_ctx, @function
set_ctx:
    movq    O_RSP(%rdi), %rsp
    movq    O_RBX(%rdi), %rbx
    movq    O_RBP(%rdi), %rbp
    movq    O_R12(%rdi), %r12
    movq    O_R13(%rdi), %r13
    movq    O_R14(%rdi), %r14
    movq    O_R15(%rdi), %r15

    movq    O_RIP(%rdi), %rcx
    pushq   %rcx

    movq    O_RSI(%rdi), %rsi
    movq    O_RDX(%rdi), %rdx
    movq    O_RCX(%rdi), %rcx
    movq    O_R8 (%rdi), %r8 
    movq    O_R9 (%rdi), %r9 

    movq    O_RDI(%rdi), %rdi

    xorl    %eax, %eax
	ret

	.p2align 4,,15
	.globl	swap_ctx
	.type	swap_ctx, @function
swap_ctx:
    movq     %rbx, O_RBX(%rdi)
    movq     %rbp, O_RBP(%rdi)
    movq     %r12, O_R12(%rdi)
    movq     %r13, O_R13(%rdi)
    movq     %r14, O_R14(%rdi)
    movq     %r15, O_R15(%rdi)

    movq     %rdi, O_RDI(%rdi)
    movq     %rsi, O_RSI(%rdi)
    movq     %rdx, O_RDX(%rdi)
    movq     %rcx, O_RCX(%rdi)
    movq     %r8 , O_R8 (%rdi)
    movq     %r9 , O_R9 (%rdi)

    movq     (%rsp), %rcx
    movq     %rcx, O_RIP(%rdi)
    leaq     8(%rsp), %rcx
    movq     %rcx, O_RSP(%rdi)

    movq    O_RSP(%rsi), %rsp
    movq    O_RBX(%rsi), %rbx
    movq    O_RBP(%rsi), %rbp
    movq    O_R12(%rsi), %r12
    movq    O_R13(%rsi), %r13
    movq    O_R14(%rsi), %r14
    movq    O_R15(%rsi), %r15

    movq    O_RIP(%rsi), %rcx
    pushq   %rcx

    movq    O_RDI(%rsi), %rdi
    movq    O_RDX(%rsi), %rdx
    movq    O_RCX(%rsi), %rcx
    movq    O_R8 (%rsi), %r8 
    movq    O_R9 (%rsi), %r9 

    movq    O_RSI(%rsi), %rsi

	xorl	%eax, %eax

	ret

	.p2align 4,,15
	.globl	start_ctx
	.type	start_ctx, @function
start_ctx:
    
	movq	%rbx, %rsp
	popq	%rdi			/* This is the next context.  */
    movq    %rbx, %rsi
	jmp	    set_ctx
	hlt
    ret

