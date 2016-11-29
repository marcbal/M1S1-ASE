#include "drive.h"
#include "vol.h"
#include "fs.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


int main() {
	srand(time(NULL));
	drive_init_material();
	printf("---> Formattage du disque...\n");
	dsknfo_s infos = drive_infos();
	for (unsigned int i = 0; i < infos.nbCylinder; i++)
		drive_format_sector(i, 0, infos.nbSector, 0);
	
	vol_drive_start();
	
	
	printf("---> Création d'un volume de taille 1023 blocs.\n");
	vol_s vol;
	vol.first.sector = 1;
	vol.first.cylinder = 0;
	vol.nbSector = 1023;
	vol.type = 0;
	
	if (!vol_add_volume(vol)) {
		return 0;
	}
	
	fs_init();
	
	fs_print_infos();
	
	
	printf("---> Creating 2 inodes ...\n");
	uint32_t inode = fs_create_inode(FILE_TYPE);
	uint32_t inode2 = fs_create_inode(FILE_TYPE);
	
	printf("---> Created inode are %u and %u\n", inode, inode2);
	
	fs_print_infos();
	
	printf("---> Deleting first inode ...\n");
	
	fs_delete_inode(inode);
	
	fs_print_infos();
	
	printf("---> Read access to 0th block of inode2 returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 0, false));
	printf("---> Write access to 0th block of inode2 returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 0, true));
	printf("---> Read access to 0th block of inode2 returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 0, false));
	printf("---> Write access to 1st block of inode2 returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 1, true));
	printf("---> Read access to 1st block of inode2 returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 1, false));
	
	
	
	fs_print_infos();
	
	printf("---> Read access to 5th block of inode2 (Indirect) returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 5, false));
	fs_print_infos();
	printf("---> Write access to 5th block of inode2 (Indirect) returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 5, true));
	printf("---> Read access to 5th block of inode2 (Indirect) returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 5, false));
	fs_print_infos();
	
	printf("---> Read access to 10th block of inode2 (Dbl-indirect) returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 10, false));
	fs_print_infos();
	printf("---> Write access to 10th block of inode2 (Dbl-indirect) returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 10, true));
	printf("---> Read access to 10th block of inode2 (Dbl-indirect) returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 10, false));
	fs_print_infos();
	printf("---> Write access to 15th block of inode2 (Dbl-indirect) returns block addr %u\n",
			fs_fileblock_to_volblock(inode2, 15, true));
	fs_print_infos();
	
	
	
	printf("---> Deleting second inode ...\n");
	
	fs_delete_inode(inode2);
	
	fs_print_infos();
	
	
}
