
# void co_ctx_swap(co_ctx_t *ctx1, co_ctx_t *ctx2)
# struct co_ctx_t
# {
# #if defined (__i386__)
#     void *regs[8];
# #else
#     void *regs[14];
# #endif
#     size_t ss_size; // 栈大小
#     char *ss_sp;    // 协程栈帧内存区域，这个区域一般在堆上分配
# };

.globl co_ctx_swap
#if !defined( __APPLE__ )
.type co_ctx_swap, @function
#endif

# 32bit下内存布局  call co_ctx_swap
#          <-    %ebp
# ...
# param 2
# param 1
# old ip         返回地址  <- %esp
# ------------------------ co_ctx_swap 执行
#

co_ctx_swap:

#if defined(__i386__)

    movl 4(%esp), %eax      # %eax -> parma 1 准确点是 (%eax) -> ctx1.regs[0]
    movl %esp   , 28(%eax)  # %esp to ctx1.regs[7]
    movl %ebp   , 24(%eax)  # %ebp to ctx1.regs[6]
    movl %esi   , 20(%eax)  # %esi to ctx1.regs[5]
    movl %edi   , 16(%eax)  # %edi to ctx1.regs[4]
    movl %edx   , 12(%eax)  # %edx to ctx1.regs[3]
    movl %ecx   ,  8(%eax)  # %ecx to ctx1.regs[2]
    movl %ebx   ,  4(%eax)  # %ebx to ctx1.regs[1]

    movl  8(%esp), %eax     # (%eax) -> ctx2.regs[0]
    movl  4(%eax), %ebx     # ctx2.regs[1] to %ebx
    movl  8(%eax), %ecx     # ctx2.regs[2] to %ecx
    movl 12(%eax), %edx     # ctx2.regs[3] to %edx
    movl 16(%eax), %edi     # ctx2.regs[4] to %edi
    movl 20(%eax), %esi     # ctx2.regs[5] to %esi
    movl 24(%eax), %ebp     # ctx2.regs[6] to %ebp
    movl 28(%eax), %esp     # ctx2.regs[7] to %esp  设置ret地址

    ret # 跳转到esp指向的地址，esp + 4

#elif defined(__x86_64__)

    leaq (%rsp), %rax       # %rsp to %rax
    movq %rax,   104(%rdi)  # %rax(%rsp) to ctx1.regs[13]
    movq %rbx,    96(%rdi)  # %rbx to ctx1.regs[12]
    movq %rcx,    88(%rdi)  # %rcx to ctx1.regs[11]
    movq %rdx,    80(%rdi)  # %rdx to ctx1.regs[10]

    movq 0(%rax), %rax      # 返回地址 to %rax
    movq %rax   , 72(%rdi)  # %rax(返回地址) to ctx1.regs[9]
    movq %rsi   , 64(%rdi)  # %rsi to ctx1.regs[8]
    movq %rdi   , 56(%rdi)  # %rdi to ctx1.regs[7]
    movq %rbp   , 48(%rdi)  # %rbp to ctx1.regs[6]
    movq %r8    , 40(%rdi)  # %r8  to ctx1.regs[5]
    movq %r9    , 32(%rdi)  # %r9  to ctx1.regs[4]
    movq %r12   , 24(%rdi)  # %r12 to ctx1.regs[3]
    movq %r13   , 16(%rdi)  # %r13 to ctx1.regs[2]
    movq %r14   ,  8(%rdi)  # %r14 to ctx1.regs[1]
    movq %r15   ,   (%rdi)  # %r15 to ctx1.regs[0]

    xorq %rax, %rax # %rax 清0

    movq  48(%rsi), %rbp    # ctx2.regs[6]  to %rbp
    movq 104(%rsi), %rsp    # ctx2.regs[13] to %rsp
    movq    (%rsi), %r15    # ctx2.regs[0]  to %r15
    movq   8(%rsi), %r14    # ctx2.regs[1]  to %r14
    movq  16(%rsi), %r13    # ctx2.regs[2]  to %r13
    movq  24(%rsi), %r12    # ctx2.regs[3]  to %r12
    movq  32(%rsi), %r9     # ctx2.regs[4]  to %r9
    movq  40(%rsi), %r8     # ctx2.regs[5]  to %r8
    movq  56(%rsi), %rdi    # ctx2.regs[7]  to %rdi
    movq  80(%rsi), %rdx    # ctx2.regs[10] to %rdx
    movq  88(%rsi), %rcx    # ctx2.regs[11] to %rcx
    movq  96(%rsi), %rbx    # ctx2.regs[12] to %rbx

    leaq 8(%rsp), %rsp
    pushq 72(%rsi)      # 两步相当于 ctx2.regs[9] to (%rsp)   设置ret地址

    movq 64(%rsi), %rsi # ctx2.regs[8] to %rsi

    ret

#endif

# 64bit下
# %rdi -> ctx1
# %rsi -> ctx2
# old ip    返回地址 <- %rsp
# ----------------------------- co_ctx_swap 执行
