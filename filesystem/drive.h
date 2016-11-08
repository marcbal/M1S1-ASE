#include <inttypes.h>


#define HDA_SECTORSIZE  64

typedef struct dsknfo_s {
	uint16_t nbCylinder, nbSector, sectorSize;
} dsknfo_s;

typedef unsigned char sector_t[HDA_SECTORSIZE];



dsknfo_s drive_infos();


void read_sector(uint16_t cylinder, uint16_t sector, sector_t* buffer);
void read_sector_n(uint16_t cylinder, uint16_t sector, void* buffer, uint16_t dataSize);
void write_sector(uint16_t cylinder, uint16_t sector, const sector_t* buffer); 
void format_sector(uint16_t cylinder, uint16_t sector, unsigned int nsector, uint32_t value);



void init_material();

void drive_print_infos();



