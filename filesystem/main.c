#include <stdlib.h>
#include <stdio.h>
#include "vol.h"





int main(int argc, char** argv) {
	vol_drive_start();
	
	
	if (vol_get_nb_volumes() < NB_MAX_VOL) {
		printf("Ajout des volumes ...\n");
		vol_s volume;
		
		volume.first.cylinder = 0;
		volume.first.sector = 1;
		volume.nbSector = 15;
		volume.type = VOL_TYPE_BASE;
		vol_add_volume(volume);
		
		volume.first.cylinder = 1;
		volume.first.sector = 0;
		volume.nbSector = 16;
		vol_add_volume(volume);
		
		volume.first.cylinder = 2;
		vol_add_volume(volume);
		
		volume.first.cylinder = 3;
		vol_add_volume(volume);
		
		volume.first.cylinder = 4;
		vol_add_volume(volume);
		
		volume.first.cylinder = 5;
		vol_add_volume(volume);
		
		volume.first.cylinder = 6;
		vol_add_volume(volume);
		
		volume.first.cylinder = 7;
		volume.nbSector = 144;
		vol_add_volume(volume);
	}
	
	/*
	printf("Suppression d'un volume\n");
	vol_remove_volume(1);
	*/
	
	vol_print_infos();
	
	return 0;
}
