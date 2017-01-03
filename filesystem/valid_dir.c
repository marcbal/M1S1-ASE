#include "drive.h"
#include "vol.h"
#include "fs.h"
#include "ftree.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>



void print_indent(int indentSize) {
	for (int i = 0; i < indentSize; i++) {
		printf("    ");
	}
}


void print_tree(uint32_t inode, int recur) {
	
	file_desc_t dir_desc;
	int ret = ftree_inode_dir_open(&dir_desc, inode);
	if (ret == OPEN_NOT_DIR_TYPE) {
		// l'inode passé en paramètre est un fichier, pas un dossier
		printf("File\n");
		return;
	}
	
	
	
	uint16_t size = ftree_inode_dir_length(&dir_desc);
	
	printf("Directory (%u elm)\n", size);
	
	for (uint16_t i=0; i<size; i++) {
		dir_entry_s entry = ftree_inode_dir_get(&dir_desc, i);
		print_indent(recur);
		printf("`- %s (inode %u): ", entry.name, entry.inode);
		
		print_tree(entry.inode, recur + 1);
		
	}
	
	
	
	
	
	
}






int main() {
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
	
	
	printf("---> Initialisation de l'arborescence. (dossier racine)\n");
	ftree_init();
	
	// ouverture dossier racine
	file_desc_t dir_desc;
	ftree_inode_dir_open(&dir_desc, fs_get_rootInode());
	
	// création des entrées dans le dossier racine
	printf("---> Création des inodes du dossier racine (dont 1 sous-dossier)\n");
	uint32_t i_1 = ifile_create(FILE_TYPE);
	uint32_t i_2 = ifile_create(DIR_TYPE);
	uint32_t i_3 = ifile_create(FILE_TYPE);
	uint32_t i_4 = ifile_create(FILE_TYPE);
	
	printf("---> Écriture des inodes dans le dossier racine\n");
	ftree_inode_dir_add(&dir_desc, "1_fichier", i_1);
	ftree_inode_dir_add(&dir_desc, "2_dossier", i_2);
	ftree_inode_dir_add(&dir_desc, "3_fichier", i_3);
	ftree_inode_dir_add(&dir_desc, "4_fichier", i_4);
	
	ftree_inode_dir_close(&dir_desc);
	
	// ouverture et remplissage du sous dossier /2_dossier/
	ftree_inode_dir_open(&dir_desc, i_2);
	printf("---> Création des inodes du sous-dossier\n");
	uint32_t i_2_1 = ifile_create(FILE_TYPE);
	uint32_t i_2_2 = ifile_create(FILE_TYPE);
	uint32_t i_2_3 = ifile_create(FILE_TYPE);
	printf("---> Écriture des inodes dans le sous-dossier\n");
	ftree_inode_dir_add(&dir_desc, "1_fichier", i_2_1);
	ftree_inode_dir_add(&dir_desc, "2_fichier", i_2_2);
	ftree_inode_dir_add(&dir_desc, "3_fichier", i_2_3);
	ftree_inode_dir_close(&dir_desc);
	
	printf("Arborescence :\n");
	printf("/ (inode %u): ", fs_get_rootInode());
	print_tree(fs_get_rootInode(), 0);
	
	
	
}
