#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"
#include "include/hardware.h"
#include "hw_config.h"


#define  N 256
#define  MMU_ENABLE 1
#define  MMU_IRQ 13

static int current_process;

static int ppage_of_vaddr(int process, int vaddr){
        unsigned vpage =(vaddr >> 12) & 0xFFF;
        unsigned ppage;

        switch (process) {
            case 0:
                    ppage = vpage +1;
            case 1:
                    ppage = vpage + N/2+1;
        }

    return ppage;
}

void mmuhandler(){
    int ppage;
    int vaddr =0;
    struct tlb_entry_s tlbe;


    _in(MMU_FAULT_ADDR);
    vaddr -= (int)virtual_memory;

    if(vaddr>= VM_SIZE)
        exit(EXIT_FAILURE);

    if(vaddr>= (N/2) * PAGE_SIZE)
        exit(EXIT_FAILURE);

    ppage = ppage_of_vaddr(current_process,vaddr);

    tlbe.tlb_vpage = (vaddr >>12);
    tlbe.tlb_ppage= ppage;
    tlbe.tlb_x= 1;
    tlbe.tlb_w= 1;
    tlbe.tlb_r= 1;
    tlbe.tlb_used= 1;

    _out(TLB_ADD_ENTRY,*((int *)&tlbe));
}

static int sum(void *ptr)
{
    int i;
    int sum = 0;

    for(i = 0; i < PAGE_SIZE * N/2 ; i++)
        sum += ((char*)ptr)[i];
    return sum;
}

static void switch_to_process0()
{
    current_process = 0;
    _out(MMU_CMD, MMU_RESET);
}

static void switch_to_process1()
{
    current_process = 1;
    _out(MMU_CMD, MMU_RESET);
}

void fvide() { };
int main(int argc, char **argv)
{
    void *ptr;
    int res;

	if (init_hardware("etc/hardware.ini\0") == 0) {
		fprintf(stderr, "Erreur lors de init_hardware()\n");
		exit(EXIT_FAILURE);
	}

	for (int i=0; i<16; i++) {
		IRQVECTOR[i] = fvide;
	}
    IRQVECTOR[16] = switch_to_process0;
    IRQVECTOR[17] = switch_to_process1;
    _mask(0x1001);

    ptr = virtual_memory;

    _int(16);
    memset(ptr, 1, PAGE_SIZE * N/2);
    _int(17);
    memset(ptr, 3, PAGE_SIZE * N/2);

    _int(16);
    res = sum(ptr);
    printf("Resultat du processus 0 : %d\n",res);
    _int(17);
    res = sum(ptr);
    printf("Resultat processus 1 : %d\n",res);
}
