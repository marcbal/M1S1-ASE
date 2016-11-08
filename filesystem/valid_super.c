#include "vol.h"
#include "super.h"
#include <stdio.h>


int main(int argc, char** argv) {
	super_init();
	
	uint32_t block;
	while ((block = super_new_block()) != 0) {
		printf("---> New block: %u\n", block);
		super_print_infos();
	}
	
	
	
	return 0;
}
