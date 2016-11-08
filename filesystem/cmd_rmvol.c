#include <stdlib.h>
#include <stdio.h>
#include "vol.h"


mbr_s mbr;


void print_usage(char** argv) {
	fprintf(stderr, "Command usage :\n");
	fprintf(stderr, "    %s <volId>\n", argv[0]);
	fprintf(stderr, "Currently %i volumes (from 0 to %i)\n", mbr.nbVol, mbr.nbVol-1);
}



int main(int argc, char** argv) {
	vol_drive_start();
	mbr = vol_get_mbr();
	
	if (mbr.nbVol == 0) {
		fprintf(stderr, "There is actually no volume to remove\n");
		return 1;
	}
	
	if (argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		print_usage(argv);
		return 1;
	}
	
	long long int volId = strtoll(argv[1], NULL, 10);
	
	if (volId < 0 || volId >= mbr.nbVol) {
		fprintf(stderr, "Fist argument must be in [0; %i]\n", mbr.nbVol-1);
		print_usage(argv);
		return 1;
	}
	
	vol_remove_volume(volId);
	return 0;
}
