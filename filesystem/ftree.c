



#include "ftree.h"

#include "ifile.h"
#include "fs.h"











void ftree_init() {
	ifile_init();
	if (fs_get_rootInode() == 0) {
		fprintf(stderr, "Creating root directory...\n");
		uint32_t rootInode = ifile_create(DIR_TYPE);
		fs_set_rootInode(rootInode);
	}
}



int ftree_inode_dir_open(file_desc_t *fd, uint32_t inode) {
	if (!ifile_open(fd, inode))
		return 0;
	if (fd->type != DIR_TYPE) {
		ifile_close(fd);
		return 0;
	}
	return 1;
}



void ftree_inode_dir_close(file_desc_t *fd) {
	ifile_close(fd);
}





uint16_t ftree_inode_dir_length(file_desc_t *fd) {
	uint16_t nbElement = 0;
	ifile_seek2(fd, 0);
	ifile_read(fd, &nbElement, sizeof(uint16_t));
	return nbElement;
}




dir_entry_s ftree_inode_dir_get(file_desc_t *fd, uint16_t index) {
	dir_entry_s entry;
	ifile_seek2(fd, sizeof(uint16_t) + index * sizeof(dir_entry_s));
	ifile_read(fd, &entry, sizeof(dir_entry_s));
	return entry;
}




void ftree_inode_dir_add(file_desc_t *fd, char* name, uint32_t inode) {
	if (strlen(name) == 0) {
		fprintf(stderr, "Can't add file with empty name");
		return;
	}
	
	
	dir_entry_s entry;
	if (strlen(name) >= FILE_NAME_SIZE) {
		memcpy(entry.name, name, FILE_NAME_SIZE - 1);
		entry.name[FILE_NAME_SIZE-1] = '\0';
	}
	else {
		memcpy(entry.name, name, strlen(name)+1);
	}
	entry.inode = inode;
	
	
	uint16_t length = ftree_inode_dir_length(fd);
	uint16_t newLength = length+1;
	ifile_seek2(fd, 0);
	ifile_read(fd, &newLength, sizeof(uint16_t));
	ifile_seek2(fd, sizeof(uint16_t) + length * sizeof(dir_entry_s));
	ifile_write(fd, &entry, sizeof(dir_entry_s));
	ifile_flush(fd);
}






