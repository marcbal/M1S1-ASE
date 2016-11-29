#include "drive.h"

int main() {
	drive_init_material();
	dsknfo_s infos = drive_infos();
	for (unsigned int i = 0; i < infos.nbCylinder; i++)
		drive_format_sector(i, 0, infos.nbSector, 0);
	return 0;
}
