#include "co_ctx.h"
#include <string>
#include <memory>
#include <stdio.h>

using namespace std;

const static int STACK_SIZE = 128*1024;

extern "C"
{
extern void co_ctx_swap(co_ctx_t*, co_ctx_t*) asm("co_ctx_swap");
}

struct stRo_t
{
    co_ctx_t *ctx;
    string content;
    void (*fn)(stRo_t *);
};

int RoutineFunc(stRo_t *cur, stRo_t *pre)
{
    int a = 0;
    if (cur->fn) {
        (*cur->fn)(cur);
    }
    printf("&a is %lx\n",(unsigned long)&a);
    co_ctx_swap(cur->ctx, pre->ctx); //第一次切回去
    if (cur->fn) {
        (*cur->fn)(cur);
    }
    printf("&a is %lx\n",(unsigned long)&a);
    co_ctx_swap(cur->ctx, pre->ctx); //第二次切回去

    printf("invalid here!\n");//走到这里来，表示错误的切换过来了，后续没有切回去的代码了,走到这里会core
    return 0;
}

void UserFunc(stRo_t *co)
{
    int a;
    printf("%s\n", co->content.c_str());
    printf("user &a is %lx\n",(unsigned long)&a);
    return;
}


int main()
{

    char *buffer = new char[2*STACK_SIZE];


    co_ctx_t main_ctx;
    main_ctx.ss_size = STACK_SIZE;
    main_ctx.ss_sp = buffer;

    co_ctx_t pending_ctx;
    pending_ctx.ss_size = STACK_SIZE;
    pending_ctx.ss_sp = buffer + STACK_SIZE;

    printf("&stack is %lx\n",(unsigned long)(pending_ctx.ss_sp+pending_ctx.ss_size));

    stRo_t pending, mainro;
    pending.ctx = &pending_ctx;
    pending.fn = UserFunc;
    pending.content = "hello";
    mainro.ctx = &main_ctx;
    mainro.fn = NULL;

    printf("main 0, before switch to hello\n");
    co_ctx_make(&pending_ctx, (co_ctx_pfn_t)RoutineFunc, &pending, &mainro);

    co_ctx_swap(mainro.ctx, pending.ctx);

    printf("main 1, switch back to main\n");

    co_ctx_swap(mainro.ctx, pending.ctx);
    printf("main 2, switch back to main\n");

    return  0;
}
