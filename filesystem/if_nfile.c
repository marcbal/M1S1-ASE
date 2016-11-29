#include "ifile.h"
#include <stdlib.h>
#include <stdio.h>

void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <type>\n", argv[0]);
	fprintf(stderr, "<type> : 0=FILE ; 1=DIR\n");
}



int main(int argc, char** argv) {
	ifile_init();
	
	if (argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		print_usage(argv);
		return 1;
	}
	
	long long int type = strtoll(argv[1], NULL, 10);
	
	if (type < 0 || type >= 2) {
		fprintf(stderr, "First argument must be in [0; 1]\n");
		print_usage(argv);
		return 1;
	}
	
	uint32_t inode = ifile_create(type);
	printf("%u\n", inode);
	
	
	
	
	file_desc_t fd;
	
	uint32_t blockSize = fs_get_volume_infos().blockSize;
	unsigned char* buffer = malloc(blockSize);
	ifile_open(&fd, inode);
	
	uint64_t totalWrite = 0;
	int nbRead, ret = 0;
	while((nbRead = fread(buffer, 1, blockSize, stdin)) > 0) {
		int nbWrite = ifile_write(&fd, buffer, nbRead);
		if (nbWrite < 0)
			break;
		totalWrite += nbWrite;
		if (nbWrite < nbRead) {
			fprintf(stderr, "WRITE_NO_FREE_SPACE error\n");
		}
	}
	fprintf(stderr, "ByteWritten: %llu\n", totalWrite);
	ifile_close(&fd);
	if (ret == WRITE_INVALID) {
		fprintf(stderr, "WRITE_INVALID error\n");
		return 1;
	}
	
	return 0;
}
