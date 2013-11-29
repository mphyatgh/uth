#ifndef __ctx_h__
#define __ctx_h__

#define O_RAX     0
#define O_RBX     8
#define O_RCX     16
#define O_RDX     24
#define O_RSI     32
#define O_RDI     40
#define O_RSP     48
#define O_RBP     56
#define O_R8      64
#define O_R9      72
#define O_R10     80
#define O_R11     88
#define O_R12     96
#define O_R13     104
#define O_R14     112
#define O_R15     120
#define O_RIP     128

#ifndef __ASSEMBLER__
//#define __USE_UCONTEXT__

#ifdef __USE_UCONTEXT__
#include <ucontext.h>
#define ctx_t       ucontext_t
#define get_ctx     getcontext
#define set_ctx     setcontext
#define swap_ctx    swapcontext
#define make_ctx    makecontext
#else /* __USE_UCONTEXT__ */

struct ctx_s {
    unsigned long   rax;
    unsigned long   rbx;
    unsigned long   rcx;
    unsigned long   rdx;
    unsigned long   rsi;
    unsigned long   rdi;
    unsigned long   rsp;
    unsigned long   rbp;
    unsigned long   r8;
    unsigned long   r9;
    unsigned long   r10;
    unsigned long   r11;
    unsigned long   r12;
    unsigned long   r13;
    unsigned long   r14;
    unsigned long   r15;
    unsigned long   rip;
    struct {
        char            *ss_sp;
        unsigned long   ss_size;
    }uc_stack;
    struct ctx_s    *uc_link;
};

typedef struct ctx_s ctx_t;

int get_ctx(ctx_t *c);
int set_ctx(ctx_t *c);
int swap_ctx(ctx_t *c, ctx_t *n);
int start_ctx(void);
static inline int make_ctx(ctx_t *c, void (*func)(void), int argc, void *argv)
{
    unsigned long   *sp, spl;

    spl = (unsigned long)(c->uc_stack.ss_sp + c->uc_stack.ss_size);
    spl &= -16L;
    spl -= 16;

    sp = (unsigned long *)spl;
    c->rip = (unsigned long)func;
    c->rbx = (unsigned long)&sp[1];
    c->rdi = (unsigned long)argv;
    c->rsp = (unsigned long)sp;
    sp[0] = (unsigned long)start_ctx;
    sp[1] = (unsigned long)c->uc_link;
    printf("rbx = %p\n", &sp[1]);
    return 0;
}

#endif /* __USE_UCONTEXT__ */

#endif /* __ASSEMBLER__ */

#endif /* __ctx_h__ */



