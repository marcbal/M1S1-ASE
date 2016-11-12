#include "vol.h"
#include "super.h"
#include <stdio.h>


int main(int argc, char** argv) {
	super_init();
	
	super_print_infos();
	
	uint32_t block;
	while ((block = super_new_block()) != 0) {
		printf("---> New block: %u\n", block);
		super_print_infos();
	}
	
	super_free_block(3);
	printf("---> Free block: 3\n");
	super_print_infos();
	
	super_free_block(2);
	printf("---> Free block: 2\n");
	super_print_infos();
	
	super_free_block(6);
	printf("---> Free block: 6\n");
	super_print_infos();
	
	super_free_block(4);
	printf("---> Free block: 4\n");
	super_print_infos();
	
	
	return 0;
}
