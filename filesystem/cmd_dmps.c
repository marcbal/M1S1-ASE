#include <stdlib.h>
#include <stdio.h>
#include "drive.h"
#include "dump.h"


dsknfo_s infos;

void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <cylinder> <sector>\n", argv[0]);
	fprintf(stderr, "- Nb cylinder: %i\n", infos.nbCylinder);
	fprintf(stderr, "- Nb sector:   %i\n", infos.nbSector);
}



int main(int argc, char** argv) {
	init_material();
	infos = drive_infos();
	
	if (argc < 3) {
		fprintf(stderr, "Not enough arguments\n");
		print_usage(argv);
		return 1;
	}
	
	long long int arg1 = strtoll(argv[1], NULL, 10);
	long long int arg2 = strtoll(argv[2], NULL, 10);
	
	if (arg1 < 0 || arg1 >= infos.nbCylinder) {
		fprintf(stderr, "Fist argument invalid, must be an integer in [0; %i]\n", infos.nbCylinder);
		print_usage(argv);
		return 1;
	}
	if (arg2 < 0 || arg2 >= infos.nbSector) {
		fprintf(stderr, "Second argument invalid, must be an integer in [0; %i]\n", infos.nbSector);
		print_usage(argv);
		return 1;
	}
	
	sector_t sector_content;
	read_sector((uint16_t)arg1, (uint16_t)arg2, &sector_content);
	dump((unsigned char*)&sector_content, infos.sectorSize, 0, 1);
	
	return 0;
}
