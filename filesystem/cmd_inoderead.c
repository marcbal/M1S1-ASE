#include "ifile.h"
#include "fs.h"
#include <stdlib.h>
#include <stdio.h>

void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <inode> [start] [length]\n", argv[0]);
	fprintf(stderr, "Print the content of the file in the std output.\n");
	fprintf(stderr, "If length is negative, the file will be read until the end is reached.\n");
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
	long long int length = (argc > 3) ? strtoll(argv[3], NULL, 10) : -1;
	
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
	fprintf(stderr, "Length: %llu\n", fd.size);
	fprintf(stderr, "Start: %lli\n", start);
	
	if (length < 0)
		length = fd.size - start;
	int targetLength = length;
	int nbRead;
	while((nbRead = ifile_read(&fd, buffer, (length < 1024) ? length : 1024)) > 0) {
		fwrite(buffer, 1, nbRead, stdout);
		length -= nbRead;
	}
	fprintf(stderr, "ByteRead: %lli\n", targetLength - length);
	fprintf(stderr, "File printed to stdout.\n");
	ifile_close(&fd);
	if (nbRead == READ_INVALID) {
		fprintf(stderr, "READ_INVALID error\n");
		return 1;
	}
	
	return 0;
}
