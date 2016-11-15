#include "vol.h"
#include "fs.h"
#include <stdio.h>


int main(int argc, char** argv) {
	fs_init();
	
	fs_print_infos();
	
	uint32_t block;
	while ((block = fs_new_block()) != 0) {
		printf("---> New block: %u\n", block);
		fs_print_infos();
	}
	
	fs_free_block(3);
	printf("---> Free block: 3\n");
	fs_print_infos();
	
	fs_free_block(2);
	printf("---> Free block: 2\n");
	fs_print_infos();
	
	fs_free_block(6);
	printf("---> Free block: 6\n");
	fs_print_infos();
	
	fs_free_block(4);
	printf("---> Free block: 4\n");
	fs_print_infos();
	
	
	return 0;
}
