
#include "context.h"






typedef struct sem_s {
	
	int    compteur;
	
    ctx_s*   ctx_curr;
} sem_s;



void sem_init(sem_s* sem, unsigned int val);

void sem_down(sem_s* sem);

void sem_up(sem_s* sem);


