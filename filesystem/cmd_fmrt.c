#include "drive.h"

int main(int argc, char** argv) {
	init_material();
	dsknfo_s infos = drive_infos();
	for (unsigned int i = 0; i < infos.nbCylinder; i++)
		format_sector(i, 0, infos.nbSector, 0);
	return 0;
}
