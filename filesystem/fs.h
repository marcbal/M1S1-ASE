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





enum file_type_e { FILE_TYPE, DIR_TYPE };

#define N_DIRECT 2
#define N_INDIRECT 5

typedef uint32_t inode_indir_s[N_INDIRECT];

typedef struct inode_s {
	enum file_type_e type;
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




uint32_t fs_create_inode(enum file_type_e type);
void fs_delete_inode(uint32_t inode);
uint32_t fs_fileblock_to_volblock(uint32_t inode, uint32_t fblock, bool allocate);

