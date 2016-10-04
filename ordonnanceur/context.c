#include "context.h"
#include <stdio.h>

#include <assert.h>
#include <stdlib.h>


ctx_s* cctx = NULL;
ctx_s mctx;

ctx_s* ring_ctx = NULL; 


void switch_to_ctx(ctx_s* ctx);

int init_ctx(ctx_s* ctx, int stack_size, func_t f, void* arg);







int init_ctx(ctx_s* ctx, int stack_size, func_t f, void* arg) {
    ctx->ctx_magic = CTX_MAGIC;
    ctx->ctx_f = f;
    ctx->ctx_arg = arg;
    ctx->ctx_stack = malloc(stack_size);
    assert(ctx->ctx_stack);
    ctx->ctx_esp = ctx->ctx_ebp = ctx->ctx_stack + stack_size - 4;
    ctx->ctx_state = CTX_INIT;
    return 0;
}




void switch_to_ctx(ctx_s* ctx) {
	if (ctx == cctx)
		return;

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
        yield();
    }

}



int create_ctx(int stack_size, func_t f, void* arg) {
	ctx_s* new_ctx = malloc(sizeof(ctx_s));
	assert(new_ctx);
	init_ctx(new_ctx, stack_size, f, arg);
	if (ring_ctx) {
		new_ctx->next = ring_ctx->next;
		ring_ctx->next = new_ctx;
	}
	else {
		ring_ctx = new_ctx;
		new_ctx->next = new_ctx;
	}
	return 0;
}





void yield() {
	if (cctx) {
		while (cctx->next->ctx_state == CTX_END) {
			if (cctx->next == cctx) {
				ring_ctx = NULL;
				asm("movl %0, %%esp" : : "r" (mctx.ctx_esp));
				asm("movl %0, %%ebp" : : "r" (mctx.ctx_ebp));
				break;
			}
			else {
				ctx_s* next_next = cctx->next->next;
				free(cctx->next->ctx_stack);
				free(cctx->next);
				cctx->next = next_next;
			}
		}
		switch_to_ctx(cctx->next);
	}
	else if (ring_ctx) {
		switch_to_ctx(ring_ctx);
		free(cctx->ctx_stack);
		free(cctx);
		cctx = NULL;
	}
}
