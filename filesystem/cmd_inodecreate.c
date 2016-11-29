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
	
	return 0;
}
