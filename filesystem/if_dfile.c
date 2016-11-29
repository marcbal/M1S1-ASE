#include "ifile.h"
#include "fs.h"
#include <stdlib.h>
#include <stdio.h>

void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <inode>\n", argv[0]);
	fprintf(stderr, "Remove the inode.\n");
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
	
	ifile_delete(inode);
	
	return 0;
}
