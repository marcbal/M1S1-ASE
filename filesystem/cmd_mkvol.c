#include <stdlib.h>
#include <stdio.h>
#include "vol.h"


void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <cylinder> <sector> <size> <type>\n", argv[0]);
	fprintf(stderr, "All arguments are unsigned.\n");
	fprintf(stderr, "<cylinder> and <sector> are 16 bits values.\n");
	fprintf(stderr, "<size> is 32 bits values.\n");
	fprintf(stderr, "<type> : 0=BASE ; 1=ANNEX ; 2=OTHER\n");
}



int main(int argc, char** argv) {
	vol_drive_start();
	
	if (argc < 5) {
		fprintf(stderr, "Not enough arguments\n");
		print_usage(argv);
		return 1;
	}
	
	long long int cylinder = strtoll(argv[1], NULL, 10);
	long long int sector = strtoll(argv[2], NULL, 10);
	long long int size = strtoll(argv[3], NULL, 10);
	long long int type = strtoll(argv[4], NULL, 10);
	
	if (cylinder < 0 || cylinder > 0xFFFF) {
		fprintf(stderr, "Fist argument overflows a 16 bits unsigned integer\n");
		print_usage(argv);
		return 1;
	}
	if (sector < 0 || sector >= 0xFFFF) {
		fprintf(stderr, "Second argument overflows a 16 bits unsigned integer\n");
		print_usage(argv);
		return 1;
	}
	if (size < 0 || size >= 0xFFFFFFFF) {
		fprintf(stderr, "Third argument overflows a 32 bits unsigned integer\n");
		print_usage(argv);
		return 1;
	}
	if (type < 0 || type >= 3) {
		fprintf(stderr, "Fourth argument must be in [0; 2]\n");
		print_usage(argv);
		return 1;
	}
	
	
	vol_s vol;
	vol.first.sector = sector;
	vol.first.cylinder = cylinder;
	vol.nbSector = size;
	vol.type = type;
	
	if (vol_add_volume(vol)) {
		printf("Volume added successfully.\n");
		return 0;
	}
	fprintf(stderr, "Can't add new Volume.\n");
	return 1;
}
