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
		volume.nbSector = 767;
		volume.type = VOL_TYPE_BASE;
		vol_add_volume(volume);
		
		volume.first.cylinder = 12;
		volume.first.sector = 0;
		volume.nbSector = 64;
		vol_add_volume(volume);
		
		volume.first.cylinder = 13;
		vol_add_volume(volume);
		
		volume.first.cylinder = 14;
		vol_add_volume(volume);
		
		volume.first.cylinder = 15;
		volume.nbSector = 32;
		vol_add_volume(volume);
		
		volume.first.sector = 32;
		vol_add_volume(volume);
	}
	
	/*
	printf("Suppression d'un volume\n");
	vol_remove_volume(1);
	*/
	
	vol_print_infos();
	
	
	vol_format_vol(3);
	
	return 0;
}
