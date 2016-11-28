#ifndef FS_H
#define FS_H

#include <inttypes.h>
#include "primtypes.h"



#define SUPER_MAGIC 0x356FA57B
#define SUPER_BLOCK 0 // address of super block
#define NO_NEXT 0




typedef struct super_s {
	uint32_t magic;
	uint32_t firstFreeBlock;
	uint32_t nbFreeBlock;
	uint32_t root;
	char name[32];
	uint32_t nbTotalBlock;
} super_s;





typedef uint8_t file_type_e;
#define FILE_TYPE 0
#define DIR_TYPE 1




#define N_DIRECT 2
#define N_INDIRECT 5

typedef uint32_t inode_indir_s[N_INDIRECT];

typedef struct inode_s {
	file_type_e type;
	uint64_t size;
	uint32_t direct[N_DIRECT];
	uint32_t indirect;
	uint32_t dbl_indirect;
} inode_s;







void     fs_init();
uint32_t fs_new_block();
void     fs_free_block(uint32_t block);
void     fs_print_infos();

uint8_t fs_get_current_volume();


typedef struct vol_info_s{
	uint32_t nbBlock;
	uint32_t freeBlock;
	uint32_t blockSize;
	uint32_t maxBlockPerFile;
} vol_info_s;

vol_info_s fs_get_volume_infos();






uint32_t fs_create_inode(file_type_e type);
void fs_delete_inode(uint32_t inode);
void fs_read_inode(uint32_t inodeNum, inode_s* inodeData);
void fs_write_inode(uint32_t inodeNum, inode_s* inodeData);
uint32_t fs_fileblock_to_volblock(uint32_t inode, uint32_t fblock, bool allocate);

#endif
