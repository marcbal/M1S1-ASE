#include <inttypes.h>


typedef struct dsknfo_s {
	uint16_t nbCylinder, nbSector, sectorSize;
} dsknfo_s;



dsknfo_s drive_infos();

void* drive_allocate_buffer();

void drive_free_buffer(void* buffer);


void drive_read_sector(uint16_t cylinder, uint16_t sector, void* buffer);
void drive_read_sector_n(uint16_t cylinder, uint16_t sector, void* buffer, uint16_t dataSize);
void drive_write_sector(uint16_t cylinder, uint16_t sector, void* buffer);
void drive_write_sector_n(uint16_t cylinder, uint16_t sector, void* buffer, uint16_t n);
void drive_format_sector(uint16_t cylinder, uint16_t sector, unsigned int nsector, uint32_t value);



void drive_init_material();

void drive_print_infos();



