#include "include/hardware.h"
#include "drive.h"
#include "primtypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define HDA_CMDREG   0x3F6
#define HDA_DATAREGS 0x110
#define HDA_IRQ      14

bool driveInit = false;


int currCylinder = -1, currSector = -1;

dsknfo_s hda_infos;


void hda_seek(uint16_t cylinder, uint16_t sector) {
	if (cylinder == currCylinder && sector == currSector)
		return;
	_out(HDA_DATAREGS,   (cylinder >> 8) & 0xff);
	_out(HDA_DATAREGS+1, (cylinder) & 0xff);
	
	_out(HDA_DATAREGS+2, (sector >> 8) & 0xff);
	_out(HDA_DATAREGS+3, (sector) & 0xff);
	
	_out(HDA_CMDREG, CMD_SEEK);
	
	_sleep(HDA_IRQ);
	currCylinder = cylinder;
	currSector = sector;
}

void hda_read_sector_n(void* buffer, uint16_t n) {
	_out(HDA_DATAREGS,   0x00);
	_out(HDA_DATAREGS+1, 0x01);
	
	_out(HDA_CMDREG, CMD_READ);
	_sleep(HDA_IRQ);
	memcpy(buffer, MASTERBUFFER, (hda_infos.sectorSize > n) ? n : hda_infos.sectorSize);
	currSector = (currSector + 1) % hda_infos.nbSector;
}

void hda_read_sector(void* buffer) {
	hda_read_sector_n(buffer, hda_infos.sectorSize);
}

void hda_format_sector(uint32_t data) {
	_out(HDA_DATAREGS,   0);
	_out(HDA_DATAREGS+1, 1);
	
	_out(HDA_DATAREGS+2, (data >> 24) & 0xff);
	_out(HDA_DATAREGS+3, (data >> 16) & 0xff);
	_out(HDA_DATAREGS+4, (data >> 8) & 0xff);
	_out(HDA_DATAREGS+5, (data) & 0xff);
	
	_out(HDA_CMDREG, CMD_FORMAT);
	
	_sleep(HDA_IRQ);
	currSector = (currSector + 1) % hda_infos.nbSector;
}

dsknfo_s drive_infos() {
	return hda_infos;
}


void hda_write_sector_n(void* buffer, uint16_t n) {
	_out(HDA_DATAREGS,   0x00);
	_out(HDA_DATAREGS+1, 0x01);
	memcpy(MASTERBUFFER, buffer, n);
	_out(HDA_CMDREG, CMD_WRITE);
	_sleep(HDA_IRQ);
	currSector = (currSector + 1) % hda_infos.nbSector;
}


void hda_write_sector(void* buffer) {
	hda_write_sector_n(buffer, hda_infos.sectorSize);
}










void drive_read_sector(uint16_t cylinder, uint16_t sector, void* buffer) {
	hda_seek(cylinder, sector);
	hda_read_sector(buffer);
}
void drive_read_sector_n(uint16_t cylinder, uint16_t sector, void* buffer, uint16_t dataSize) {
	hda_seek(cylinder, sector);
	hda_read_sector_n(buffer, dataSize);
}
void drive_write_sector(uint16_t cylinder, uint16_t sector, void* buffer) {
	hda_seek(cylinder, sector);
	hda_write_sector(buffer);
}
void drive_write_sector_n(uint16_t cylinder, uint16_t sector, void* buffer, uint16_t n) {
	hda_seek(cylinder, sector);
	hda_write_sector_n(buffer, n);
}
void drive_format_sector(uint16_t cylinder, uint16_t sector, unsigned int nsector, uint32_t value) {
	hda_seek(cylinder, sector);
	for (uint32_t i=0; i<nsector; i++)
		hda_format_sector(value);
}














void fvide() { }

void drive_init_material() {
	
	if (driveInit) return;
	driveInit = true;
	
	char* HARDWARE_INI = getenv("HW_CONFIG");
	if (HARDWARE_INI == NULL) {
		HARDWARE_INI = "etc/hardware.ini\0";
	}
	
	if (init_hardware(HARDWARE_INI) == 0) {
		fprintf(stderr, "Erreur lors de init_hardware()\n");
		exit(EXIT_FAILURE);
	}
	
	for (int i=0; i<16; i++) {
		IRQVECTOR[i] = fvide;
	}
	
	// init info from disk
	
	_out(HDA_CMDREG, CMD_DSKINFO);
	hda_infos.nbCylinder = (_in(HDA_DATAREGS    ) << 8) + _in(HDA_DATAREGS + 1);
	hda_infos.nbSector   = (_in(HDA_DATAREGS + 2) << 8) + _in(HDA_DATAREGS + 3);
	hda_infos.sectorSize = (_in(HDA_DATAREGS + 4) << 8) + _in(HDA_DATAREGS + 5);
	
}



void drive_print_infos() {
	
	printf("HDA Infos:\n");
	printf("- Nb cylinder:      %i\n", hda_infos.nbCylinder);
	printf("- Nb sector/cyl:    %i\n", hda_infos.nbSector);
	printf("- Sector size:      %i\n", hda_infos.sectorSize);
	int totSec = hda_infos.nbSector * hda_infos.nbCylinder;
	printf("- Total nb sector:  %i\n", totSec);
	printf("- Total size:       %i bytes\n", totSec * hda_infos.sectorSize);
}




void* drive_allocate_buffer() {
	return malloc(hda_infos.sectorSize);
}

void drive_free_buffer(void* buffer) {
	free(buffer);
}
