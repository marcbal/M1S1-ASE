
#include "fs.h"
#include "vol.h"
#include "drive.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct free_s {
	uint32_t nbBlock;
	uint32_t next; // 0 = pas de next. Car 0 c'est l'adresse du super block
} free_s;




uint8_t VOLUME_ID;

dsknfo_s diskInfo;

vol_s volume;

super_s super;





bool fsInit = false;





/*
 * Struture de données en mémoire de la liste chainé
 * des portions d'espace libre
 */

typedef struct free_node {
	uint32_t block;
	free_s node;
	struct free_node* next;
} free_node;

typedef struct free_list {
	free_node* first;
} free_list;

free_list _free_list_instance;

void free_list_init() {
	if (super.firstFreeBlock == NO_NEXT) {
		_free_list_instance.first = NULL;
		return;
	}
	
	free_s free;
	free_node* previous_node = NULL;
	free_node* node = NULL;
	for (uint32_t free_block = super.firstFreeBlock;
			free_block != NO_NEXT;
			free_block = free.next) {
		vol_read_bloc_n(VOLUME_ID, free_block, &free, sizeof(free_s));
		
		
		node = (free_node*) malloc(sizeof(free_node));
		node->block = free_block;
		node->node = free;
		if (previous_node == NULL)
			_free_list_instance.first = node;
		else
			previous_node->next = node;
		previous_node = node;
	}
	node->next = NULL;
}

// structure uniquement utilisée comme valeur de retour de la
// fonction qui suit
typedef struct free_block_taken {
	uint32_t block;
	uint32_t newFirstFree;
	uint8_t  free_needToWrite;
	free_s   free_data;
} free_block_taken;

free_block_taken free_list_take_first_free() {
	free_block_taken ret = {0, 0, 0, {0, 0}};
	if (_free_list_instance.first == NULL) {
		return ret;
	}
	
	ret.block = _free_list_instance.first->block;
	
	free_node* node = _free_list_instance.first;
	if (node->node.nbBlock == 1) {
		_free_list_instance.first = node->next;
		free(node);
		if (_free_list_instance.first != NULL)
			ret.newFirstFree = _free_list_instance.first->block;
		else
			ret.newFirstFree = 0;
		ret.free_needToWrite = 0;
	}
	else {
		node->block++;
		node->node.nbBlock--;
		ret.newFirstFree = node->block;
		ret.free_needToWrite = 1;
		ret.free_data = node->node;
	}
	
	return ret;
}

// structure uniquement utilisée comme valeur de retour de la
// fonction qui suit
typedef struct free_block_put {
	uint8_t  isFreeable;
	uint32_t newFirstFree;
	uint8_t  free_writeAt0;
	free_s   free_data0;
	uint8_t  free_writeAt1;
	free_s   free_data1;
} free_block_put;

free_block_put free_list_put_free(uint32_t block) {
	free_block_put ret = {0, 0, 0, {0, 0}, 0, {0, 0}};
	if (_free_list_instance.first == NULL
			|| block < _free_list_instance.first->block - 1) {
		// ajout en tête de liste
		free_node* node = (free_node*) malloc(sizeof(free_node));
		node->block = block;
		node->next = _free_list_instance.first;
		node->node.nbBlock = 1;
		node->node.next = (node->next != NULL) ? node->next->block : NO_NEXT;
		_free_list_instance.first = node;
		ret.isFreeable = 1;
		ret.newFirstFree = block;
		ret.free_writeAt0 = block;
		ret.free_data0 = node->node;
		return ret;
	}
	if (block == _free_list_instance.first->block - 1) {
		// on agrandi vers la gauche la première série de bloc libre
		_free_list_instance.first->block = block;
		_free_list_instance.first->node.nbBlock++;
		ret.isFreeable = 1;
		ret.newFirstFree = block;
		ret.free_writeAt0 = block;
		ret.free_data0 = _free_list_instance.first->node;
		return ret;
	}
	
	// on parcours les séries de blocs libres
	for (free_node* node = _free_list_instance.first;
			node != NULL;
			node = node->next) {
		if (block >= node->block && block < node->block + node->node.nbBlock) {
			// le bloc est déjà libre (dans la série de bloc libre)
			return ret;
		}
		
		if (block == node->block + node->node.nbBlock // se trouve juste après la série courante
				// mais pas juste avant une éventuelle série suivante
				&& (node->next == NULL || block < node->next->block - 1)) {
			// on agrandi vers la droite la série courante, de 1 bloc
			node->node.nbBlock++;
			ret.isFreeable = 1;
			ret.newFirstFree = _free_list_instance.first->block;
			ret.free_writeAt0 = node->block;
			ret.free_data0 = node->node;
			return ret;
		}
		
		if (block == node->block + node->node.nbBlock // se trouve juste après la série courante
				// et AUSSI juste avant la série suivante
				&& block == node->next->block - 1) {
			// on fusionne avec les deux séries avec lesquels on est en contact
			node->node.nbBlock += node->next->node.nbBlock + 1;
			node->node.next = node->next->node.next;
			free_node* toRemove = node->next;
			node->next = node->next->next;
			free(toRemove);
			ret.isFreeable = 1;
			ret.newFirstFree = _free_list_instance.first->block;
			ret.free_writeAt0 = node->block;
			ret.free_data0 = node->node;
			return ret;
		}
		
		if (node->next == NULL || block < node->next->block - 1) {
			// on ne touche ni à la série courante, ni à la série suivante
			free_node* newNode = (free_node*) malloc(sizeof(free_node));
			newNode->block = block;
			newNode->node.nbBlock = 1;
			newNode->node.next = node->node.next;
			newNode->next = node->next;
			node->next = newNode;
			node->node.next = block;
			ret.isFreeable = 1;
			ret.newFirstFree = _free_list_instance.first->block;
			ret.free_writeAt0 = node->block;
			ret.free_data0 = node->node;
			ret.free_writeAt1 = newNode->block;
			ret.free_data1 = newNode->node;
			return ret;
		}
		
		if (block == node->next->block - 1) {
			// on ne touche que la série suivante
			node->node.next--;
			node->next->block--;
			node->next->node.nbBlock++;
			ret.isFreeable = 1;
			ret.newFirstFree = _free_list_instance.first->block;
			ret.free_writeAt0 = node->block;
			ret.free_data0 = node->node;
			ret.free_writeAt1 = node->next->block;
			ret.free_data1 = node->next->node;
			return ret;
		}
	}
	
	// ne devrait jamais arriver là
	return ret;
}


/* ************************************************ */














void fs_save_super() {
	void* buffer = vol_allocate_buffer();
	memcpy(buffer, &super, sizeof(super_s));
	vol_write_bloc(VOLUME_ID, SUPER_BLOCK, buffer);
	vol_free_buffer(buffer);
}



void fs_save_free(uint32_t block, free_s freeBlock) {
	void* buffer = vol_allocate_buffer();
	memcpy(buffer, &freeBlock, sizeof(free_s));
	vol_write_bloc(VOLUME_ID, block, buffer);
	vol_free_buffer(buffer);
}






void fs_init() {
	
	if (fsInit) return;
	fsInit = true;
	
	const char* CURRENT_VOLUME = getenv("CURRENT_VOLUME");
	if (CURRENT_VOLUME == NULL) {
		fprintf(stderr, "Environment variable CURRENT_VOLUME not found.\n");
		exit(1);
	}
	
	
	vol_drive_start();
	
	diskInfo = drive_infos();
	
	assert(diskInfo.sectorSize >= sizeof(super_s));
	assert(diskInfo.sectorSize >= sizeof(free_s));
	assert(diskInfo.sectorSize >= sizeof(inode_s));
	assert(diskInfo.sectorSize >= sizeof(inode_indir_s));
	
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
		
		fs_save_super();
		
		free_s firstFreeBlock;
		firstFreeBlock.nbBlock = volume.nbSector-1;
		firstFreeBlock.next = NO_NEXT;
		
		fs_save_free(1, firstFreeBlock);
	}
	
	// lecture de l'espace libre pour mise en cache
	free_list_init();
	
	
}






void fs_print_infos() {
	printf("Volume %u of HDA:\n", VOLUME_ID);
	printf("- Total space: %u bytes - %u blocks\n", super.nbTotalBlock * diskInfo.sectorSize, super.nbTotalBlock);
	printf("- Free space: %u bytes - %u blocks\n",  super.nbFreeBlock  * diskInfo.sectorSize, super.nbFreeBlock);
	
	for (free_node* node = _free_list_instance.first;
			node != NULL;
			node = node->next) {
		printf("    - At block %u, %u blocks free\n", node->block, node->node.nbBlock);
	}
}











int fs_is_block_id_valid(uint32_t block) {
	if (block == 0) return 0;
	if (block >= volume.nbSector) return 0;
	return 1;
}





uint8_t fs_get_current_volume() {
	return VOLUME_ID;
}






uint32_t fs_new_block() {
	if (super.nbFreeBlock == 0)
		return 0;
	
	// d'abord, récupérer les infos de mise à jour du cache
	// pour l'appliquer au disque et retourner l'adresse du
	// bloc libre
	free_block_taken block_taken_data = free_list_take_first_free();
	
	// on vérifie la cohérence du cache
	if (super.firstFreeBlock != block_taken_data.block) {
		fprintf(stderr, "Unsynchronization between real free space and cached free space");
		exit(1);
	}
	
	// on applique les modifs du cache, vers le disque
	super.firstFreeBlock = block_taken_data.newFirstFree;
	super.nbFreeBlock--;
	fs_save_super();
	
	if (block_taken_data.free_needToWrite) {
		fs_save_free(super.firstFreeBlock, block_taken_data.free_data);
	}
	
	// on retourne l'adresse du bloc libéré
	return block_taken_data.block;
	
}





void fs_free_block(uint32_t block) {
	if (!fs_is_block_id_valid(block))
		return;
	
	/*// en mode aveugle on fait pas gaffe si on libère un bloc déjà libéré
	free_s free;
	free.nbBlock = 1;
	free.next = super.firstFreeBlock;
	super.firstFreeBlock = block;
	super.nbFreeBlock++;
	
	fs_save_free(block, free);
	
	fs_save_super();
	
	return;
	// -------------------*/
	
	
	
	free_block_put free_block_put_ret = free_list_put_free(block);
	
	if (!free_block_put_ret.isFreeable)
		return; // le bloc indiqué en paramètre est déjà libre. Rien à faire
	
	super.nbFreeBlock++;
	
	if (super.firstFreeBlock != free_block_put_ret.newFirstFree) {
		super.firstFreeBlock = free_block_put_ret.newFirstFree;
		fs_save_super();
	}
	
	if (free_block_put_ret.free_writeAt0 != 0) {
		fs_save_free(free_block_put_ret.free_writeAt0, free_block_put_ret.free_data0);
	}
	
	if (free_block_put_ret.free_writeAt1 != 0) {
		fs_save_free(free_block_put_ret.free_writeAt1, free_block_put_ret.free_data1);
	}
	
	
	
	
}


























void fs_read_inode(uint32_t inodeNum, inode_s* inodeData) {
	vol_read_bloc_n(VOLUME_ID, inodeNum, inodeData, sizeof(inode_s));
}


void fs_write_inode(uint32_t inodeNum, inode_s* inodeData) {
	vol_write_bloc_n(VOLUME_ID, inodeNum, inodeData, sizeof(inode_s));
}



uint32_t fs_create_inode(enum file_type_e type) {
	uint32_t inode = fs_new_block();
	inode_s data;
	data.type = type;
	data.size = 0;
	for (int i=0; i<N_DIRECT; i++)
		data.direct[i] = 0;
	data.indirect = 0;
	data.dbl_indirect = 0;
	fs_write_inode(inode, &data);
	return inode;
}


void fs_delete_inode(uint32_t inode) {
	inode_s data;
	fs_read_inode(inode, &data);
	fs_free_block(inode);
	for (int i=0; i<N_DIRECT; i++)
		fs_free_block(data.direct[i]);
	if (data.indirect != 0) {
		inode_indir_s indir;
		vol_read_bloc_n(VOLUME_ID, data.indirect, &indir, sizeof(inode_indir_s));
		for (int i=0; i<N_INDIRECT; i++)
			fs_free_block(indir[i]);
	}
	if (data.dbl_indirect != 0) {
		inode_indir_s dbl_indir;
		vol_read_bloc_n(VOLUME_ID, data.dbl_indirect, &dbl_indir, sizeof(inode_indir_s));
		for (int i=0; i<N_INDIRECT; i++) {
			inode_indir_s indir;
			vol_read_bloc_n(VOLUME_ID, dbl_indir[i], &indir, sizeof(inode_indir_s));
			for (int j=0; j<N_INDIRECT; j++)
				fs_free_block(indir[j]);
			fs_free_block(dbl_indir[i]);
		}
	}
}





uint32_t fs_allocate_datablock() {
	uint32_t block = fs_new_block();
	if (block != 0) {
		vol_format_block(VOLUME_ID, block);
	}
	return block;
}

uint32_t fs_allocate_inode_indirect(inode_indir_s* indir) {
	uint32_t block = fs_new_block();
	for (int i=0; i<N_INDIRECT; i++) {
		(*indir)[i] = 0;
	}
	if (block != 0) {
		vol_write_bloc_n(VOLUME_ID, block, indir, sizeof(inode_indir_s));
	}
	return block;
}



uint32_t fs_fileblock_to_volblock(uint32_t inode, uint32_t fblock, bool allocate) {
	inode_s inodeData;
	fs_read_inode(inode, &inodeData);
	
	
	if (fblock < N_DIRECT) {
		if (allocate && inodeData.direct[fblock] == 0) {
			inodeData.direct[fblock] = fs_allocate_datablock();
			if (inodeData.direct[fblock] != 0)
				fs_write_inode(inode, &inodeData);
		}
		return inodeData.direct[fblock];
	}
	
	fblock -= N_DIRECT;
	
	if (fblock < N_INDIRECT) {
		inode_indir_s indir;
		if (inodeData.indirect == 0) {
			if (!allocate) return 0;
			inodeData.indirect = fs_allocate_inode_indirect(&indir);
			if (inodeData.indirect != 0)
				fs_write_inode(inode, &inodeData);
		}
		else {
			vol_read_bloc_n(VOLUME_ID, inodeData.indirect, &indir, sizeof(inode_indir_s));
		}
		
		if (inodeData.indirect == 0)
			return 0;
		
		if (indir[fblock] == 0 && allocate) {
			indir[fblock] = fs_allocate_datablock();
			if (indir[fblock] != 0)
				vol_write_bloc_n(VOLUME_ID, inodeData.indirect, &indir, sizeof(inode_indir_s));
		}
		
		return indir[fblock];
	}
	
	fblock -= N_INDIRECT;
	
	if (fblock < N_INDIRECT * N_INDIRECT) {
		inode_indir_s dbl_indir;
		if (inodeData.dbl_indirect == 0) {
			if (!allocate) return 0;
			inodeData.dbl_indirect = fs_allocate_inode_indirect(&dbl_indir);
			if (inodeData.dbl_indirect != 0)
				fs_write_inode(inode, &inodeData);
		}
		else {
			vol_read_bloc_n(VOLUME_ID, inodeData.dbl_indirect, &dbl_indir, sizeof(inode_indir_s));
		}
		
		if (inodeData.dbl_indirect == 0)
			return 0;
		
		uint32_t indirBlock = fblock / N_INDIRECT;
		uint32_t dirBlock = fblock % N_INDIRECT;
		
		inode_indir_s indir;
		if (dbl_indir[indirBlock] == 0) {
			if (!allocate) return 0;
			dbl_indir[indirBlock] = fs_allocate_inode_indirect(&indir);
			if (dbl_indir[indirBlock] != 0)
				vol_write_bloc_n(VOLUME_ID, inodeData.dbl_indirect, &dbl_indir, sizeof(inode_indir_s));
		}
		else {
			vol_read_bloc_n(VOLUME_ID, dbl_indir[indirBlock], &indir, sizeof(inode_indir_s));
		}
		
		if (dbl_indir[indirBlock] == 0)
			return 0;
			
		if (indir[dirBlock] == 0 && allocate) {
			indir[dirBlock] = fs_allocate_datablock();
			if (indir[dirBlock] != 0)
				vol_write_bloc_n(VOLUME_ID, dbl_indir[indirBlock], &indir, sizeof(inode_indir_s));
		}
		
		return indir[dirBlock];
	}
	
	return 0; // bloc demandé trop grand
		
}










