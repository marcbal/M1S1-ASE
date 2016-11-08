#include <inttypes.h>



#define SUPER_MAGIC 0x356FA57B
#define SUPER_BLOCK 0 // address of super block
#define NO_NEXT 0




typedef struct super_s {
	uint32_t magic;
	uint32_t firstFreeBlock;
	uint32_t nbFreeBlock;
	uint32_t root;
	char name[32];
	uint32_t nbTotalBlock;
} super_s;


typedef struct free_s {
	uint32_t nbBlock;
	uint32_t next; // 0 = pas de next. Car 0 c'est l'adresse du super block
} free_s;







void     super_init();
uint32_t super_new_block();
void     super_free_block();
void     super_print_infos();
