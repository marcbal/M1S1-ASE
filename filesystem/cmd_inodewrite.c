#include "ifile.h"
#include "fs.h"
#include <stdlib.h>
#include <stdio.h>

void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <inode> [start]\n", argv[0]);
	fprintf(stderr, "Read the content from stdin and write it to the file, starting to the position 'start'.\n");
}



int main(int argc, char** argv) {
	ifile_init();
	
	if (argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		print_usage(argv);
		return 1;
	}
	
	long long int inode = strtoll(argv[1], NULL, 10);
	long long int start = (argc > 2) ? strtoll(argv[2], NULL, 10) : 0;
	
	if (inode <= 0 || inode >= fs_get_volume_infos().nbBlock) {
		fprintf(stderr, "Fist argument is not a valid inode. Must be in [1, %u]\n", (fs_get_volume_infos().nbBlock-1));
		print_usage(argv);
		return 1;
	}
	if (start < 0) {
		fprintf(stderr, "Second argument must be positive\n");
		print_usage(argv);
		return 1;
	}
	
	file_desc_t fd;
	
	
	unsigned char buffer[1024];
	ifile_open(&fd, inode);
	if (start != 0)
		ifile_seek2(&fd, start);
		
	fprintf(stderr, "Inode: %u\n", fd.inode);
	fprintf(stderr, "CurrentLength: %llu\n", fd.size);
	fprintf(stderr, "StartAt: %lli\n", start);
	
	uint64_t totalWrite = 0;
	int nbRead, ret = 0;
	while((nbRead = fread(buffer, 1, 1024, stdin)) > 0) {
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
