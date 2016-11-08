#include <stdlib.h>
#include <stdio.h>
#include "vol.h"



int main(int argc, char** argv) {
	vol_drive_start();
	vol_print_infos();
	return 0;
}
