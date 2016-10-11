#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>


#define N 100                         /* nombre de places dans le tampon */

sem_s mutex, vide, plein;


typedef int objet_t;

int cpt = 0;

void produire_objet(objet_t* t) {
	*t = (objet_t) cpt++;
}

void utiliser_objet(objet_t t) {
	printf("utiliser_objet: %i", t);
}

void mettre_objet(objet_t t) {
	printf("mettre_objet: %i", t);
}

void retirer_objet(objet_t* t) {
	printf("retirer_objet: %i", *t);
}



void producteur (void* arg)
{
  objet_t objet ;

  while (1) {
    produire_objet(&objet);           /* produire l'objet suivant */
    sem_down(&vide);                  /* dec. nb places libres */
    sem_down(&mutex);                 /* entree en section critique */
    mettre_objet(objet);              /* mettre l'objet dans le tampon */
    sem_up(&mutex);                   /* sortie de section critique */
    sem_up(&plein);                   /* inc. nb place occupees */
  }
}

void consommateur (void* arg)
{
  objet_t objet ;

  while (1) {
    sem_down(&plein);                 /* dec. nb emplacements occupes */
    sem_down(&mutex);                 /* entree section critique */
    retirer_objet (&objet);           /* retire un objet du tampon */
    sem_up(&mutex);                   /* sortie de la section critique */
    sem_up(&vide);                    /* inc. nb emplacements libres */
    utiliser_objet(objet);            /* utiliser l'objet */
  }
}
  











int main(int argc, char *argv[])
{
	sem_init(&mutex, 1);                /* controle d'acces au tampon */
	sem_init(&vide, N);                 /* nb de places libres */
	sem_init(&plein, 0);                /* nb de places occupees */
	create_ctx(1638400, producteur, NULL);
	create_ctx(1638400, consommateur, NULL);
	yield();
	printf("Retour au main\n");

}

