#ifndef FTREE_H
#define FTREE_H


#include <inttypes.h>
#include "primtypes.h"



#define FILE_NAME_SIZE 60


#define OPEN_NOT_DIR_TYPE -3
#define OPEN_NOT_FILE_TYPE -4




typedef struct {
	char name[FILE_NAME_SIZE];
	uint32_t inode;
} dir_entry_s;









void ftree_init();






#endif
