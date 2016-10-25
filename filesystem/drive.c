#include "include/hardware.h"
#include "drive.h"
#include "dump.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define HDA_CMDREG   0x3F6
#define HDA_DATAREGS 0x110
#define HDA_IRQ      14
#define HARDWARE_INI "etc/hardware.ini"



void hda_seek(uint16_t cylinder, uint16_t sector) {
	_out(HDA_DATAREGS,   (cylinder >> 8) & 0xff);
	_out(HDA_DATAREGS+1, (cylinder) & 0xff);
	
	_out(HDA_DATAREGS+2, (sector >> 8) & 0xff);
	_out(HDA_DATAREGS+3, (sector) & 0xff);
	
	_out(HDA_CMDREG, CMD_SEEK);
	
	_sleep(HDA_IRQ);
}

void hda_read_sector(sector_t* buffer) {
	_out(HDA_DATAREGS,   0x00);
	_out(HDA_DATAREGS+1, 0x01);
	
	_out(HDA_CMDREG, CMD_READ);
	_sleep(HDA_IRQ);
	memcpy(buffer, MASTERBUFFER, HDA_SECTORSIZE);
}

void hda_read_sector_n(void* buffer, uint16_t n) {
	_out(HDA_DATAREGS,   0x00);
	_out(HDA_DATAREGS+1, 0x01);
	
	_out(HDA_CMDREG, CMD_READ);
	_sleep(HDA_IRQ);
	memcpy(buffer, MASTERBUFFER, (HDA_SECTORSIZE > n) ? n : HDA_SECTORSIZE);
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
}

dsknfo_s drive_infos() {
	_out(HDA_CMDREG, CMD_DSKINFO);
	
	
	dsknfo_s ret;
	
	ret.nbCylinder = (_in(HDA_DATAREGS    ) << 8) + _in(HDA_DATAREGS + 1);
	ret.nbSector   = (_in(HDA_DATAREGS + 2) << 8) + _in(HDA_DATAREGS + 3);
	ret.sectorSize = (_in(HDA_DATAREGS + 4) << 8) + _in(HDA_DATAREGS + 5);
	
	return ret;
}




void hda_write_sector(sector_t* buffer) {
	_out(HDA_DATAREGS,   0x00);
	_out(HDA_DATAREGS+1, 0x01);
	memcpy(MASTERBUFFER, buffer, HDA_SECTORSIZE);
	_out(HDA_CMDREG, CMD_WRITE);
	_sleep(HDA_IRQ);
}









void read_sector(uint16_t cylinder, uint16_t sector, sector_t* buffer) {
	hda_seek(cylinder, sector);
	hda_read_sector((sector_t*)buffer);
}
void read_sector_n(uint16_t cylinder, uint16_t sector, void* buffer, uint16_t dataSize) {
	hda_seek(cylinder, sector);
	hda_read_sector_n(buffer, dataSize);
}
void write_sector(uint16_t cylinder, uint16_t sector, const sector_t* buffer) {
	hda_seek(cylinder, sector);
	hda_write_sector((sector_t*)buffer);
}
void format_sector(uint16_t cylinder, uint16_t sector, unsigned int nsector, uint32_t value) {
	hda_seek(cylinder, sector);
	for (uint32_t i=0; i<nsector; i++)
		hda_format_sector(value);
}







void dmps(uint16_t cylinder, uint16_t sector) {
	sector_t sector_content;
	read_sector(cylinder, sector, &sector_content);
	dump((unsigned char*)&sector_content, HDA_SECTORSIZE, 0, 1);
}





void fmrt() {
	dsknfo_s infos = drive_infos();
	
	for (unsigned int i = 0; i < infos.nbCylinder; i++)
		format_sector(i, 0, infos.nbSector, 0);
}







void fvide() { }

void init_material() {
	
	if (init_hardware(HARDWARE_INI) == 0) {
		fprintf(stderr, "Erreur lors de init_hardware()\n");
		exit(EXIT_FAILURE);
	}
	
	for (int i=0; i<16; i++) {
		IRQVECTOR[i] = fvide;
	}
	
	// IRQVECTOR[HDA_IRQ] = ;
	//_out(TIMER_PARAM, 128+64+32+8);
	//irq_enable();
	
}



void drive_print_infos() {
	dsknfo_s infos = drive_infos();
	
	printf("HDA Infos:\n");
	printf("- Nb cylinder:      %i\n", infos.nbCylinder);
	printf("- Nb sector/cyl:    %i\n", infos.nbSector);
	printf("- Sector size:      %i\n", infos.sectorSize);
	int totSec = infos.nbSector * infos.nbCylinder;
	printf("- Total nb sector:  %i\n", totSec);
	printf("- Total size:       %i bytes\n", totSec * infos.sectorSize);
}
