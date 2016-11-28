


#include "ifile.h"

#include "vol.h"




uint32_t ifile_next_file_desc_id = 0;

uint32_t ifile_buffer_size;




bool ifile_initialized = false;

void ifile_init() {
	if (ifile_initialized)
		return;
	ifile_initialized = true;
	
	fs_init();
	
	ifile_buffer_size = fs_get_volume_infos().blockSize;
	
}




bool ifile_inode_valid(uint32_t inode) {
	return inode > 0 && inode < fs_get_volume_infos().nbBlock;
}

bool ifile_file_desc_valid(file_desc_t* descriptor) {
	return ifile_inode_valid(descriptor->inode)
}





uint32_t ifile_create(enum file_type_e type) {
	return fs_create_inode(type);
}


int ifile_delete(uint32_t inumber) {
	if (!ifile_inode_valid(inumber))
		return 0;
	fs_delete_inode(inumber);
	return 1;
}


/*
 * private function to use when read, write or seek operations
 * 
 * it manage buffer save/load when changing block
 * and values in the descriptor
 * 
 * @return true if we have changed block, false otherwise.
 */
bool ifile_change_position(file_desc_t* descriptor, uint32_t newPosition) {
	uint32_t oldPos = descriptor->currentPos;
	uint32_t oldBufferIndex = descriptor->bufferIndex;
	if (newPosition == oldPosition)
		return false;
	
	uint32_t newBufferIndex = newPosition / ifile_buffer_size;
	uint32_t newPosInBuffer = newPosition % ifile_buffer_size;
	
	if (newBufferIndex != oldBufferIndex) {
		// on change de block
		ifile_flush(descriptor);
		
		
		descriptor->bufferPos = newBufferIndex * ifile_buffer_size;
		descriptor->bufferIndex = newBufferIndex;
		descriptor->bufferBlock = 0;
		free(descriptor->buffer);
		descriptor->buffer = NULL;
		descriptor->bufferModified = false;
		
	}
	descriptor->currentPos = newPosition;
	descriptor->currentPosInBuffer = newPosInBuffer;
	
}




int ifile_open(file_desc_t *fd, uint32_t inumber) {
	if (!ifile_inode_valid(inumber))
		return 0;
	
	inode_s inodeData;
	fs_read_inode(inumber, &inodeData);
	
	fd->id = ifile_next_file_desc_id++;
	fd->inode = inumber;
	fd->type = inodeData.type;
	fd->size = inodeData.size;
	fd->currentPos = fd->currentPosInBuffer
					= fd->bufferPos
					= fd->bufferIndex
					= fd->bufferBlock
					= 0;
	fd->buffer = NULL;
	fd->bufferModified = false;
	
	return 1;
}




void ifile_flush(file_desc_t *fd) {
	if (!ifile_file_desc_valid(fd))
		return;
	if (fd->buffer == NULL || !fd->bufferModified) {
		return;
	}
	vol_write_bloc(fs_get_current_volume(), fd->bufferBlock, fd->buffer);
	fd->bufferModified = false;
}



void ifile_seek2(file_desc_t *fd, uint64_t a_offset) { /* absolu */
	if (!ifile_file_desc_valid(fd))
		return;
	ifile_change_position(fd, a_offset);
}


void ifile_seek(file_desc_t *fd, int64_t r_offset) { /* relatif */
	if (!ifile_file_desc_valid(fd))
		return;
	if (-r_offset > fd->currentPos)
		r_offset = -fd->currentPos;
	ifile_change_position(fd, fd->currentPos + r_offset
}


void ifile_close(file_desc_t *fd) {
	if (!ifile_file_desc_valid(fd))
		return;
	
	ifile_flush(fd);
	free(descriptor->buffer);
	descriptor->buffer = NULL;
	descriptor->inode = 0; // rend le descripteur invalide
	
	
}





int ifile_readc(file_desc_t *fd) {
	if (!ifile_file_desc_valid(fd))
		return READ_INVALID;
	if (fd->currentPos >= fd->size)
		return READ_EOF;
	if (fd->buffer == NULL) {
		fd->buffer = malloc(ifile_buffer_size);
		fd->bufferBlock = fs_fileblock_to_volblock(fd->inode, fd->bufferIndex, false);
		if (fd->bufferBlock == 0) {
			memset(fd->buffer, 0, ifile_buffer_size);
		}
		else {
			vol_read_bloc(fs_get_current_volume(), fd->bufferBlock, fd->buffer);
		}
	}
	unsigned char v = (unsigned char) fd->buffer[fd->currentPosInBuffer];
	ifile_change_position(fd, fd->currentPos + 1);
	return (int) v;
}

int ifile_writec(file_desc_t *fd, char c) {
	if (!ifile_file_desc_valid(fd))
		return WRITE_INVALID;
	if (fd->buffer == NULL || fd->bufferBlock == 0) {
		fd->bufferBlock = fs_fileblock_to_volblock(fd->inode, fd->bufferIndex, true);
		if (fd->bufferBlock == 0) {
			return WRITE_NO_FREE_SPACE;
		}
		if (fd->buffer == NULL) {
			fd->buffer = malloc(ifile_buffer_size);
			vol_read_bloc(fs_get_current_volume(), fd->bufferBlock, fd->buffer);
		}
	}
	fd->buffer[fd->currentPosInBuffer] = c;
	fd->bufferModified = true;
	if (fd->currentPos >= fd->size)
	ifile_change_position(fd, fd->currentPos + 1);
	return 0;
}



