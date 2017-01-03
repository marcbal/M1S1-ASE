#ifndef FTREE_H
#define FTREE_H


#include <inttypes.h>
#include "primtypes.h"

#include "ifile.h"



#define FILE_NAME_SIZE 60


#define OPEN_NOT_DIR_TYPE -3
#define OPEN_NOT_FILE_TYPE -4




typedef struct {
	char name[FILE_NAME_SIZE];
	uint32_t inode;
} dir_entry_s;









void ftree_init();


int ftree_inode_dir_open(file_desc_t *fd, uint32_t inode);
void ftree_inode_dir_close(file_desc_t *fd);
uint16_t ftree_inode_dir_length(file_desc_t *fd);
dir_entry_s ftree_inode_dir_get(file_desc_t *fd, uint16_t index);
void ftree_inode_dir_add(file_desc_t *fd, char* name, uint32_t inode);
void free_inode_dir_remove(file_desc_t *fd, uint16_t index);




#endif
