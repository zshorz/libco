
#include "co_ctx.h"

#include <stdio.h>
#include <string.h>

// 寄存器位置下标
#define ESP 0
#define EIP 1
#define EAX 2
#define ECX 3
// -----------
#define RSP 0
#define RIP 1
#define RBX 2
#define RDI 3
#define RSI 4

#define RBP 5
#define R12 6
#define R13 7
#define R14 8
#define R15 9
#define RDX 10
#define RCX 11
#define R8  12
#define R9  13

// 32 bit
// regs[0] : ret
// regs[1] : ebx
// regs[2] : ecx
// regs[3] : edx
// regs[4] : edi
// regs[5] : esi
// regs[6] : ebp
// regs[7] : esp
enum {
    kEIP = 0,
    kEBP = 6,
    kESP = 7,
};

// 64 bit
// regs[0]  : r15
// regs[1]  : r14
// regs[2]  : r13
// regs[3]  : r12
// regs[4]  : r9
// regs[5]  : r8
// regs[6]  : rbp
// regs[7]  : rdi
// regs[8]  : rsi
// regs[9]  : ret    ret func addr
// regs[10] : rdx
// regs[11] : rcx
// regs[12] : rbx
// regs[13] : rsp
enum {
    kRDI = 7,
    kRSI = 8,
    kRETADDR = 9,
    kRSP = 13,
};


// 该函数汇编实现
extern "C"
{
extern void co_ctx_swap(co_ctx_t*, co_ctx_t*) asm("co_ctx_swap");
}

#if defined (__i386__) // 32 bit

int co_ctx_init(co_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    return 0;
}

int co_ctx_make(co_ctx_t *ctx, co_ctx_pfn_t pfn, const void *s, const void *s1)
{
    // 把堆上分配的数组末尾当做栈顶， 从而由高地址向低地址扩展
    char *sp = ctx->ss_sp + ctx->ss_size - sizeof(co_ctx_param_t);
    sp = (char*)((unsigned long)sp & -16L);   //  sp&(...ff ff ff7aca010f f0) 末尾4个bit置0

    co_ctx_param_t *param = (co_ctx_param_t*)sp;
    void **ret_addr = (void**)(sp - sizeof(void*) * 2);
    *ret_addr = (void*)pfn; // 返回地址 目标函数地址
    param->s1 = s;
    param->s2 = s1;

    memset(ctx->regs, 0, sizeof(ctx->regs));
    ctx->regs[kESP] = (char*)(sp) - sizeof(void*) * 2;

    // 为什么 是 - sizeof(void*) * 2
    // ctx 栈的情况
    // arg2
    // arg1
    // (NULL)
    // ret addr(pfn)  <- %esp (co_ctx_swap把%esp指向这里)然后 co_ctx_swap 中会发生一次 ret esp会+4
    // 就变成了正常的调用了 pfn, 注意pfn不能返回，因为 %esp 指向的内存为NULL
    // arg2
    // arg1
    // (NULL)         <- new %esp
    // ---------------- pfn run ---------

    return 0;
}

#elif defined (__x86_64__) // 64 bit

int co_ctx_init(co_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    return 0;
}

// 64位下，函数调用，寄存器约定这些与32位下有很大不同
int co_ctx_make(co_ctx_t *ctx, co_ctx_pfn_t pfn, const void *s, const void *s1)
{
    // 把堆上分配的数组末尾当做栈顶， 从而由高地址向低地址扩展
    char *sp = ctx->ss_sp + ctx->ss_size - sizeof(void*);
    sp = (char*)((unsigned long)sp & -16LL);

    memset(ctx->regs, 0, sizeof(ctx->regs));
    void **ret_addr = (void**)(sp);
    *ret_addr = (void*)pfn;

    ctx->regs[kRSP] = sp;
    ctx->regs[kRETADDR] = (char*)pfn;
    ctx->regs[kRDI] = (char*)s; // 64位下传参使用寄存器（前提是参数少，多的部分依然压栈）
    ctx->regs[kRSI] = (char*)s1;

    return 0;
}

#endif




