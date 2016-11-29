#include "drive.h"
#include "vol.h"
#include "fs.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


int main() {
	srand(time(NULL));
	drive_init_material();
	printf("Formattage du disque...\n");
	dsknfo_s infos = drive_infos();
	for (unsigned int i = 0; i < infos.nbCylinder; i++)
		drive_format_sector(i, 0, infos.nbSector, 0);
	
	vol_drive_start();
	
	
	printf("Création d'un volume de taille 10 blocs.\n");
	vol_s vol;
	vol.first.sector = 1;
	vol.first.cylinder = 0;
	vol.nbSector = 10;
	vol.type = 0;
	
	if (!vol_add_volume(vol)) {
		return 0;
	}
	
	
	fs_init();
	
	fs_print_infos();
	
	uint32_t block;
	while ((block = fs_new_block()) != 0) {
		printf("---> New block: %u\n", block);
	}
	
	printf("---> Tous les blocs sont pris :\n");
	fs_print_infos();
	
	int max = rand()%10+5;
	printf("---> on effectue %i libérations de blocs\n", max);
	for (int i=0; i<max; i++) {
		uint32_t numBlock = rand()%9+1;
		printf("---> libération du bloc %i\n", numBlock);
		fs_free_block(numBlock);
	}
	
	fs_print_infos();
	
	printf("---> On réalloue le plus de blocs possible\n");
	int count = 0;
	while ((block = fs_new_block()) != 0) {
		count++;
	}
	
	printf("---> %i blocs ont pu ête alloués avant de ne plus avoir de place\n", count);
	
	
	
	return 0;
}
