
struct tlb_entry_s{
    unsigned tlb_rfu;
    unsigned tlb_vpage;
    unsigned tlb_ppage;
    unsigned tlb_x;
    unsigned tlb_w;
    unsigned tlb_r;
    unsigned tlb_used;
};

void mmuhandler();
static int ppage_of_vaddr(int process , int vaddr);
