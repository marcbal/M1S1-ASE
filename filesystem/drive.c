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


typedef struct dsknfo_s {
	uint16_t nbCylinder, nbSector, sectorSize;
} dsknfo_s;

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

void hda_format(uint16_t nbSectors, uint32_t data) {
	_out(HDA_DATAREGS,   (nbSectors >> 8) & 0xff);
	_out(HDA_DATAREGS+1, (nbSectors) & 0xff);
	
	_out(HDA_DATAREGS+2, (data >> 24) & 0xff);
	_out(HDA_DATAREGS+3, (data >> 16) & 0xff);
	_out(HDA_DATAREGS+4, (data >> 8) & 0xff);
	_out(HDA_DATAREGS+5, (data) & 0xff);
	
	_out(HDA_CMDREG, CMD_FORMAT);
	
	_sleep(HDA_IRQ);
}

dsknfo_s hda_dsknfo() {
	_out(HDA_CMDREG, CMD_DSKINFO);
	
	
	dsknfo_s ret;
	
	ret.nbCylinder = (_in(HDA_DATAREGS    ) << 8) + _in(HDA_DATAREGS + 1);
	ret.nbSector   = (_in(HDA_DATAREGS + 2) << 8) + _in(HDA_DATAREGS + 3);
	ret.sectorSize = (_in(HDA_DATAREGS + 4) << 8) + _in(HDA_DATAREGS + 5);
	
	return ret;
}

uint16_t hda_manuf() {
	_out(HDA_CMDREG, CMD_MANUF);
	
	return (_in(HDA_DATAREGS) << 8) + _in(HDA_DATAREGS + 1);
}




void dmps(uint16_t cylinder, uint16_t sector) {
	hda_seek(cylinder, sector);
	sector_t sector_content;
	hda_read_sector(&sector_content);
	dump((unsigned char*)&sector_content, HDA_SECTORSIZE, 0, 1);
}

void hda_write_sector(sector_t* buffer) {
	_out(HDA_DATAREGS,   0x00);
	_out(HDA_DATAREGS+1, 0x01);
	_out(HDA_CMDREG, CMD_WRITE);
	_sleep(HDA_IRQ);
	memcpy(MASTERBUFFER, buffer, HDA_SECTORSIZE);
}




void fmrt() {
	dsknfo_s infos = hda_dsknfo();
	
	for (int i=0; i<infos.nbCylinder; i++) {
		for (int j=0; j<infos.nbSector; j++) {
			hda_seek(i, j);
			hda_format(1, 0);
		}
	}
	
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
