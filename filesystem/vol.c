#include "vol.h"

#include "drive.h"

#include "dump.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "primtypes.h"




bool volInit = false;



mbr_s mbr;
dsknfo_s dsknfo;



uint32_t getAbsoluteSector(cyl_sec_s cyl_sec) {
	return cyl_sec.cylinder * dsknfo.nbSector + cyl_sec.sector;
}

cyl_sec_s getCylAndSec(uint32_t absoluteSector) {
	cyl_sec_s ret;
	ret.cylinder = absoluteSector / dsknfo.nbSector;
	ret.sector = absoluteSector % dsknfo.nbSector;
	return ret;
}


void vol_read_mbr() {
	drive_read_sector_n(0, 0, &mbr, sizeof(mbr_s));
}

void vol_write_mbr() {
	void* buffer = drive_allocate_buffer();
	memcpy(buffer, &mbr, sizeof(mbr_s));
	drive_write_sector(0, 0, buffer);
	drive_free_buffer(buffer);
}


void vol_init_mbr() {
	mbr.nbVol = 0;
	mbr.magic = MBR_MAGIC;
	vol_write_mbr();
}




mbr_s vol_get_mbr() {
	return mbr;
}




void vol_drive_start() {
	
	if (volInit) return;
	volInit = true;
	
	drive_init_material();
	
	assert(drive_infos().sectorSize >= sizeof(mbr_s));
	dsknfo = drive_infos();
	vol_read_mbr();
	if (mbr.magic != MBR_MAGIC) {
		printf("Formatting MBR ...\n");
		vol_init_mbr();
	}
	
	
}




cyl_sec_s getCylAndSecFromBlock(uint8_t vol, uint32_t block) {
	uint32_t firstAbsSec = getAbsoluteSector(mbr.volumes[vol].first);
	uint32_t absSecOfBlock = firstAbsSec + block;
	return getCylAndSec(absSecOfBlock);
}





/**
 * Retourne le numéro de volume qui occupe le secteur passé en paramètre,
 * ou -1 si il n'y en a pas
 */
int getVolumeAtAbsoluteSector(uint32_t absSec) {
	for (int j = 0; j < mbr.nbVol; j++) {
		uint32_t firstVolJ = getAbsoluteSector(mbr.volumes[j].first);
		uint32_t lastVolJ = firstVolJ + mbr.volumes[j].nbSector - 1;
		if (absSec >= firstVolJ && absSec <= lastVolJ) {
			return j;
		}
	}
	return -1;
}






void vol_print_infos() {
	drive_print_infos();
	
	printf("MBR Volumes infos (%i/%i):\n", mbr.nbVol, NB_MAX_VOL);
	
	for (int i = 0; i < mbr.nbVol; i++) {
		uint32_t absSec = getAbsoluteSector(mbr.volumes[i].first);
		uint32_t lastAbsSec = absSec + mbr.volumes[i].nbSector - 1;
		cyl_sec_s lastCylSec = getCylAndSec(lastAbsSec);
		printf("- Volume %i: %i bytes - ",
				i, mbr.volumes[i].nbSector * drive_infos().sectorSize);
		printf("%i blocks - ", mbr.volumes[i].nbSector);
		printf("(%i, %i) abs %i -> ",
				mbr.volumes[i].first.cylinder, mbr.volumes[i].first.sector, absSec);
		printf("(%i, %i) abs %i - ",
				lastCylSec.cylinder, lastCylSec.sector, lastAbsSec);
		if (mbr.volumes[i].type == VOL_TYPE_BASE)
			printf("BASE\n");
		else if (mbr.volumes[i].type == VOL_TYPE_ANNX)
			printf("ANNX\n");
		else
			printf("OTHER\n");
	}
	
	printf("HDA Volumes map: 1 char = 1 sector ; 1 line = 1 cylinder\n  M");
	for (uint32_t absSec = 1; absSec < dsknfo.nbSector * dsknfo.nbCylinder; absSec++) {
		if (absSec % dsknfo.nbSector == 0)
			printf("\n  ");
		int vol = getVolumeAtAbsoluteSector(absSec);
		if (vol == -1) printf("-");
		else printf("%i", vol);
	}
	printf("\n");
	
	
}



int vol_add_volume(vol_s volume) {
	if (mbr.nbVol == NB_MAX_VOL) {
		fprintf(stderr, "Le nombre maximum de volume a été atteint\n");
		return 0;
	}
	
	if (volume.first.cylinder >= dsknfo.nbCylinder) {
		fprintf(stderr, "Le numéro de cylindre est trop grand\n");
		return 0;
	}
	if (volume.first.sector >= dsknfo.nbSector) {
		fprintf(stderr, "Le numéro de secteur est trop grand\n");
		return 0;
	}
	if (volume.first.sector == 0 && volume.first.cylinder  == 0) {
		fprintf(stderr, "Le volume ne peut pas écraser le MBR\n");
		return 0;
	}
	uint32_t firstAbsSec = getAbsoluteSector(volume.first);
	uint32_t lastAbsSec = firstAbsSec + volume.nbSector - 1;
	if (lastAbsSec >= dsknfo.nbCylinder * dsknfo.nbSector) {
		fprintf(stderr, "Le volume dépasse la fin du disque\n");
		return 0;
	}
	
	for (uint32_t i = firstAbsSec; i <= lastAbsSec; i++) {
		if (getVolumeAtAbsoluteSector(i) != -1) {
			fprintf(stderr, "Le volume risque d'écraser un autre volume\n");
			return 0;
		}
	}
	
	mbr.volumes[mbr.nbVol] = volume;
	mbr.nbVol++;
	vol_write_mbr();
	
	drive_format_sector(volume.first.cylinder, volume.first.sector, 1, 0);
	
	return 1;
}



int vol_remove_volume(uint8_t vol) {
	if (vol >= mbr.nbVol) {
		fprintf(stderr, "Le volume demandé n'existe pas\n");
		return 0;
	}
	
	
	for (uint8_t i = vol; i < mbr.nbVol - 1; i++) {
		mbr.volumes[i] = mbr.volumes[i+1];
	}
	
	mbr.nbVol--;
	vol_write_mbr();
	
	return 1;
}



uint8_t vol_get_nb_volumes() {
	return mbr.nbVol;
}


uint32_t vol_get_nb_blocks(uint8_t vol) {
	if (vol >= mbr.nbVol) {
		fprintf(stderr, "Le volume demandé n'existe pas\n");
		return 0;
	}
	return mbr.volumes[vol].nbSector;
}






int vol_valid_vol_block(uint8_t vol, uint32_t nbloc) {
	if (vol >= mbr.nbVol) {
		fprintf(stderr, "Le volume demandé n'existe pas\n");
		return 0;
	}
	
	vol_s* volume = &(mbr.volumes[vol]);
	
	if (nbloc >= volume->nbSector) {
		fprintf(stderr, "Le numéro de bloc demandé est trop grand\n");
		return 0;
	}
	
	return 1;
}




void vol_read_bloc(uint8_t vol, uint32_t nbloc, void* buffer) {
	if (!vol_valid_vol_block(vol, nbloc)) return;
	
	cyl_sec_s cylSec = getCylAndSecFromBlock(vol, nbloc);
	
	drive_read_sector(cylSec.cylinder, cylSec.sector, (void*)buffer);
	
}


void vol_read_bloc_n(uint8_t vol, uint32_t nbloc, void* buffer, uint16_t dataSize) {
	if (!vol_valid_vol_block(vol, nbloc)) return;
	
	cyl_sec_s cylSec = getCylAndSecFromBlock(vol, nbloc);
	
	drive_read_sector_n(cylSec.cylinder, cylSec.sector, buffer, dataSize);
}



void vol_write_bloc(uint8_t vol, uint32_t nbloc, void* buffer) {
	if (!vol_valid_vol_block(vol, nbloc)) return;
	
	cyl_sec_s cylSec = getCylAndSecFromBlock(vol, nbloc);
	
	drive_write_sector(cylSec.cylinder, cylSec.sector, buffer);
	
}



void vol_write_bloc_n(uint8_t vol, uint32_t nbloc, void* buffer, uint16_t n) {
	if (!vol_valid_vol_block(vol, nbloc)) return;
	
	cyl_sec_s cylSec = getCylAndSecFromBlock(vol, nbloc);
	
	drive_write_sector_n(cylSec.cylinder, cylSec.sector, buffer, n);
	
}



void vol_format_block(uint8_t vol, uint32_t nbloc) {
	if (!vol_valid_vol_block(vol, nbloc)) return;
	
	cyl_sec_s cylSec = getCylAndSecFromBlock(vol, nbloc);
	
	drive_format_sector(cylSec.cylinder, cylSec.sector, 1, 0);
}



void vol_format_vol(uint8_t vol) {
	if (!vol_valid_vol_block(vol, 0)) return;
	
	for (uint32_t i = 0; i < mbr.volumes[vol].nbSector; i++) {
		cyl_sec_s cylSec = getCylAndSecFromBlock(vol, i);
		drive_format_sector(cylSec.cylinder, cylSec.sector, 1, 0);
	}
	
}





void* vol_allocate_buffer() {
	return drive_allocate_buffer();
}

void vol_free_buffer(void* buffer) {
	drive_free_buffer(buffer);
}


