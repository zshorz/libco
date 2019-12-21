
#ifndef CO_CTX_H
#define CO_CTX_H

#include <stdlib.h>

typedef void* (*co_ctx_pfn_t)(void *s, void *s2);

struct co_ctx_param_t
{
    const void *s1;
    const void *s2;
};

struct co_ctx_t
{
#if defined (__i386__)
    void *regs[8];
#else
    void *regs[14];
#endif
    size_t ss_size; // 栈大小
    char *ss_sp;    // 协程栈帧内存区域，这个区域一般在堆上分配
};

int co_ctx_init(co_ctx_t *ctx);
int co_ctx_make(co_ctx_t *ctx, co_ctx_pfn_t pfn, const void *s, const void *s1);

#endif // CO_CTX_H
