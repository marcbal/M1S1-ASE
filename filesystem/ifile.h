#ifndef IFILE_H
#define IFILE_H

#include <inttypes.h>
#include "primtypes.h"


#include "fs.h"

#define READ_EOF -1
#define READ_INVALID -2
#define WRITE_NO_FREE_SPACE -1
#define WRITE_INVALID -2


typedef struct file_desc_t{
	uint32_t id;
	uint32_t inode;
	file_type_e type;
	
	uint64_t size;
	
	uint64_t currentPos; // current position in the file, in byte
	uint32_t currentPosInBuffer; // current position in the current buffer, in byte
	uint64_t bufferPos; // position, in byte from the begining of the file, of the first byte of the buffer
	uint32_t bufferIndex; // block index of the current buffer in the file
	uint32_t bufferBlock; // block position of the current buffer in the volume
	void* buffer;
	bool bufferModified;
} file_desc_t;





uint32_t ifile_create(enum file_type_e type); 
int ifile_delete(uint32_t inumber);

int ifile_open(file_desc_t *fd, uint32_t inumber);
void ifile_close(file_desc_t *fd);
void ifile_flush(file_desc_t *fd);
void ifile_seek(file_desc_t *fd, int64_t r_offset);  /* relatif */
void ifile_seek2(file_desc_t *fd, uint64_t a_offset); /* absolu */

int ifile_readc(file_desc_t *fd);
int ifile_read(file_desc_t *fd, void *buf, unsigned int nbyte);
int ifile_writec(file_desc_t *fd, char c);
int ifile_write(file_desc_t *fd, const void *buf, unsigned int nbyte);

#endif
