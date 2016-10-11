
#include <stdlib.h>
#include "semaphore.h"



void sem_init(sem_s* sem, unsigned int val) {
	sem->compteur = val;
	sem->ctx_curr = NULL;
}

void sem_down(sem_s* sem) {
	// irq_disable();
	sem->compteur--;
	if (sem->compteur < 0) {
		cctx->sem_next_ctx = sem->ctx_curr;
		sem->ctx_curr = cctx;
		cctx->ctx_state = CTX_SEM;
		// irq_enable();
		yield();
	}
	else {
		// irq_enable();
	}
}

void sem_up(sem_s* sem) {
	// irq_disable();
	sem->compteur++;
	if (sem->compteur <= 0) {
		sem->ctx_curr->ctx_state = CTX_EXEC;
		sem->ctx_curr = sem->ctx_curr->sem_next_ctx;
	}
	// irq_enable();
}
