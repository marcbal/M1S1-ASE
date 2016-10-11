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
		// irq_enable();
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
	// irq_disable();
	if (cctx) {
		ctx_s* tmp_ctx = cctx;
		while (tmp_ctx->next->ctx_state == CTX_END || tmp_ctx->next->ctx_state == CTX_SEM) {
			
			if (tmp_ctx->next->ctx_state == CTX_END) {
				if (tmp_ctx->next == cctx) {
					ring_ctx = NULL;
					// irq_enable();
					asm("movl %0, %%esp" : : "r" (mctx.ctx_esp));
					asm("movl %0, %%ebp" : : "r" (mctx.ctx_ebp));
					break;
				}
				else {
					ctx_s* next_next = tmp_ctx->next->next;
					free(tmp_ctx->next->ctx_stack);
					free(tmp_ctx->next);
					tmp_ctx->next = next_next;
				}
			}
			else { // CTX_SEM
				tmp_ctx = tmp_ctx->next;
				if (tmp_ctx == cctx) {
					fprintf(stderr, "Deadlock detected. Returning to main...\n");
				}
			}
		}
		switch_to_ctx(tmp_ctx->next);
	}
	else if (ring_ctx) {
		switch_to_ctx(ring_ctx);
		free(cctx->ctx_stack);
		free(cctx);
		cctx = NULL;
	}
	// irq_enable();
}





void irq_enable() {
	_mask(1);
	_out(TIMER_ALARM, 0xFFFFFFFE);
}

void irq_disable() {
	_mask(15);
}


void fvide() { }
