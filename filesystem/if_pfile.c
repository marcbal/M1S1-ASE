#include "ifile.h"
#include "fs.h"
#include <stdlib.h>
#include <stdio.h>

void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <inode>\n", argv[0]);
	fprintf(stderr, "Print the content of the file in the std output.\n");
}



int main(int argc, char** argv) {
	ifile_init();
	
	if (argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		print_usage(argv);
		return 1;
	}
	
	long long int inode = strtoll(argv[1], NULL, 10);
	
	if (inode <= 0 || inode >= fs_get_volume_infos().nbBlock) {
		fprintf(stderr, "Fist argument is not a valid inode. Must be in [1, %u]\n", (fs_get_volume_infos().nbBlock-1));
		print_usage(argv);
		return 1;
	}
	
	file_desc_t fd;
	
	
	uint32_t blockSize = fs_get_volume_infos().blockSize;
	unsigned char* buffer = malloc(blockSize);
	ifile_open(&fd, inode);
		
	fprintf(stderr, "Inode: %u\n", fd.inode);
	fprintf(stderr, "Length: %llu\n", fd.size);
	
	int nbRead;
	uint64_t nbTotalRead = 0;
	while((nbRead = ifile_read(&fd, buffer, blockSize)) > 0) {
		fwrite(buffer, 1, nbRead, stdout);
		nbTotalRead += nbRead;
	}
	fprintf(stderr, "ByteRead: %lli\n", nbTotalRead);
	fprintf(stderr, "File printed to stdout.\n");
	ifile_close(&fd);
	if (nbRead == READ_INVALID) {
		fprintf(stderr, "READ_INVALID error\n");
		return 1;
	}
	
	return 0;
}
