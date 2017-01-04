#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>


#define N 5                         /* nombre de places dans le tampon */

sem_s mutex, vide, plein;



typedef int objet_t;


/*
 * Structure de donnée servant de file entre le producteur et le consommateur
 */
struct obj_el {
	objet_t          value;
    struct obj_el*   next;
};

typedef struct obj_queue {
    struct obj_el*   first;
    struct obj_el*   last;
} obj_queue;

obj_queue queue;

void mettre_objet(objet_t t) {
	printf("mettre_objet: %i\n", t);
	struct obj_el* el = malloc(sizeof(struct obj_el));
	el->value = t;
	el->next = NULL;
	if (queue.first == NULL) {
		queue.first = queue.last = el;
	}
	else {
		queue.last->next = el;
		queue.last = el;
	}
}

void retirer_objet(objet_t* t) {
	if (queue.first != NULL) {
		*t = queue.first->value;
		struct obj_el* old = queue.first;
		queue.first = queue.first->next;
		free(old);
	}
	printf("retirer_objet: %i\n", *t);
	
}






void produire_objet(objet_t* t) {
	static int cpt = 0;
	*t = (objet_t) cpt++;
}

void utiliser_objet(objet_t t) {
	// printf("utiliser_objet: %i\n", t);
}








void producteur (void* arg)
{
  objet_t objet = 0;

  do {
    objet++;           /* produire l'objet suivant */
    sem_down(&vide);                  /* dec. nb places libres */
    sem_down(&mutex);                 /* entree en section critique */
    printf("%i ", *((int*)arg));
    mettre_objet(objet);              /* mettre l'objet dans le tampon */
    sem_up(&mutex);                   /* sortie de section critique */
    sem_up(&plein);                   /* inc. nb place occupees */
  } while (objet < 1000); // provoque un deadlock
}

void consommateur (void* arg)
{
  objet_t objet ;

  while (1) {
    sem_down(&plein);                 /* dec. nb emplacements occupes */
    sem_down(&mutex);                 /* entree section critique */
    retirer_objet(&objet);            /* retire un objet du tampon */
    sem_up(&mutex);                   /* sortie de la section critique */
    sem_up(&vide);                    /* inc. nb emplacements libres */
    utiliser_objet(objet);            /* utiliser l'objet */
  }
}
  








int main(int argc, char *argv[])
{
	
	if (init_hardware(HARDWARE_INI) == 0) {
		fprintf(stderr, "Erreur lors de init_hardware()\n");
		exit(EXIT_FAILURE);
	}
	
	
	
	for (int i=0; i<16; i++) {
		IRQVECTOR[i] = fvide;
	}
	
	IRQVECTOR[TIMER_IRQ] = yield;
	_out(TIMER_PARAM, 128+64+32+8);
	irq_enable();
	
	
	
	
	
	
	
	queue.first = NULL;
	queue.last = NULL;
	sem_init(&mutex, 1);                /* controle d'acces au tampon */
	sem_init(&vide, N);                 /* nb de places libres */
	sem_init(&plein, 0);                /* nb de places occupees */
	int un = 1, deux = 2, trois = 3;
	create_ctx(1638400, producteur, &un);
	create_ctx(1638400, producteur, &deux);
	create_ctx(1638400, producteur, &trois);
	create_ctx(1638400, consommateur, NULL);
	yield();
	printf("Retour au main\n");

}

