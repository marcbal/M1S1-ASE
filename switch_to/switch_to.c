#include "switch_to.h"

#include <assert.h>
#include <stdlib.h>


struct ctx_s* cctx = (struct ctx_s*) 0;
struct ctx_s mctx;

int init_ctx(struct ctx_s* ctx, int stack_size, func_t f, void* arg) {
    ctx->ctx_magic = CTX_MAGIC;
    ctx->ctx_f = f;
    ctx->ctx_arg = arg;
    ctx->ctx_stack = malloc(stack_size);
    assert(ctx->ctx_stack);
    ctx->ctx_esp = ctx->ctx_ebp = ctx->ctx_stack + stack_size - 4;
    return 0;
}

void switch_to_ctx(struct ctx_s* ctx) {

    assert(ctx->ctx_magic == CTX_MAGIC);
    if (cctx) {
        asm("movl %%esp, %0" : "=r" (cctx->ctx_esp));
        asm("movl %%ebp, %0" : "=r" (cctx->ctx_ebp));
    }
    else {
        asm("movl %%esp, %0" : "=r" (mctx.ctx_esp));
        asm("movl %%ebp, %0" : "=r" (mctx.ctx_ebp));
    }

    cctx = ctx;

    asm("movl %0, %%esp" : : "r" (cctx->ctx_esp));
    asm("movl %0, %%ebp" : : "r" (cctx->ctx_ebp));

    if (cctx->ctx_state == CTX_INIT) {
        cctx->ctx_state = CTX_EXEC;
        cctx->ctx_f(cctx->ctx_arg);
        cctx->ctx_state = CTX_END;
        asm("movl %0, %%esp" : : "r" (mctx.ctx_esp));
        asm("movl %0, %%ebp" : : "r" (mctx.ctx_ebp));
    }

}
