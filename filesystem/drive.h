#include <inttypes.h>


#define HDA_SECTORSIZE  32


typedef unsigned char sector_t[HDA_SECTORSIZE];


void dmps(uint16_t cylinder, uint16_t sector);


void fmrt();

void init_material();





