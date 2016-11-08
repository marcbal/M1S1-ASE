
#include "super.h"
#include "vol.h"
#include "drive.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


uint8_t VOLUME_ID;

dsknfo_s diskInfo;

vol_s volume;

super_s super;




void super_save_super() {
	void* buffer = vol_allocate_buffer();
	memcpy(buffer, &super, sizeof(super_s));
	vol_write_bloc(VOLUME_ID, SUPER_BLOCK, buffer);
	vol_free_buffer(buffer);
}



void super_save_free(uint32_t block, free_s freeBlock) {
	void* buffer = vol_allocate_buffer();
	memcpy(buffer, &freeBlock, sizeof(free_s));
	vol_write_bloc(VOLUME_ID, block, buffer);
	vol_free_buffer(buffer);
}






void super_init() {
	
	const char* CURRENT_VOLUME = getenv("CURRENT_VOLUME");
	if (CURRENT_VOLUME == NULL) {
		fprintf(stderr, "Environment variable CURRENT_VOLUME not found.\n");
		exit(1);
	}
	
	
	vol_drive_start();
	
	diskInfo = drive_infos();
	
	assert(diskInfo.sectorSize >= sizeof(super_s));
	assert(diskInfo.sectorSize >= sizeof(free_s));
	
	if (vol_get_mbr().nbVol == 0) {
		fprintf(stderr, "There is no volume in HDA. Please create 1 volume to continue.\n");
		exit(1);
	}
	
	// on vérifie la variable d'env CURRENT_VOLUME
	
	long long int env_volume_id = strtoll(CURRENT_VOLUME, NULL, 10);
	if (env_volume_id < 0 || env_volume_id >= vol_get_mbr().nbVol) {
		fprintf(stderr, "Environment variable CURRENT_VOLUME is out of range.\n");
		fprintf(stderr, "%u volumes are in HDA (from 0 to %u)\n", vol_get_mbr().nbVol, vol_get_mbr().nbVol-1);
		exit(1);
	}
	
	VOLUME_ID = env_volume_id;
	
	
	volume = vol_get_mbr().volumes[VOLUME_ID];
	
	// lecture de super sur le volume
	vol_read_bloc_n(VOLUME_ID, SUPER_BLOCK, &super, sizeof(super_s));
	
	// si super n'est pas initialisé
	if (super.magic != SUPER_MAGIC || super.nbTotalBlock != volume.nbSector) {
		printf("Initializing Filesystem on volume %i\n", VOLUME_ID);
		super.magic = SUPER_MAGIC;
		super.firstFreeBlock = 1;
		super.nbFreeBlock = volume.nbSector-1;
		super.nbTotalBlock = volume.nbSector;
		super.root = 0;
		super.name[0] = '\0';
		
		super_save_super();
		
		free_s firstFreeBlock;
		firstFreeBlock.nbBlock = volume.nbSector-1;
		firstFreeBlock.next = NO_NEXT;
		
		super_save_free(1, firstFreeBlock);
	}
	
	
}






void super_print_infos() {
	printf("Volume %u of HDA:\n", VOLUME_ID);
	printf("- Total space: %u bytes - %u blocks\n", super.nbTotalBlock * diskInfo.sectorSize, super.nbTotalBlock);
	printf("- Free space: %u bytes - %u blocks\n",  super.nbFreeBlock  * diskInfo.sectorSize, super.nbFreeBlock);
	
	free_s free;
	for (uint32_t free_block = super.firstFreeBlock;
			free_block != NO_NEXT;
			free_block = free.next) {
		vol_read_bloc_n(VOLUME_ID, free_block, &free, sizeof(free_s));
		printf("    - At block %u, %u blocks free\n", free_block, free.nbBlock);
	}
}











int super_is_block_id_valid(uint32_t block) {
	if (block == 0) return 0;
	if (block >= volume.nbSector) return 0;
	return 1;
}












uint32_t super_new_block() {
	if (super.nbFreeBlock == 0)
		return 0;
	
	uint32_t returned = super.firstFreeBlock;
	
	free_s free;
	vol_read_bloc_n(VOLUME_ID, returned, &free, sizeof(free_s));
	if (free.nbBlock == 1) {
		// super block pointe sur free.next
		super.firstFreeBlock = free.next;
	}
	else {
		free.nbBlock--;
		super.firstFreeBlock++;
		super_save_free(super.firstFreeBlock, free);
	}
	
	super.nbFreeBlock--;
	
	super_save_super();
	
	return returned;
	
}





void super_free_block(uint32_t block) {
	if (!super_is_block_id_valid(block))
		return;
	
	// en mode aveugle on fait pas gaffe si on libère un bloc déjà libéré
	free_s free;
	free.nbBlock = 1;
	free.next = super.firstFreeBlock;
	super.firstFreeBlock = block;
	super.nbFreeBlock++;
	
	super_save_free(block, free);
	
	super_save_super();
	
	return;
	// -------------------
	
	
	
	
	
	/*
	
	if (super.firstFreeBlock == NO_NEXT) {
		// aucun bloc n'est libre sur le volume
		super.firstFreeBlock = block;
		super.nbFreeBlock++;
		
		super_save_super();
		
		free_s free;
		free.nbBlock = 1;
		free.next = NO_NEXT;
		
		super_save_free(block, free);
		return;
	}
	
	
	
	
	*/
	
	
	
	
	
}


