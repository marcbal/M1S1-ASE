#include <stdlib.h>
#include <assert.h>

struct ctx_s {
    void* ctx_esp;
    void* ctx_ebp;
    unsigned ctx_magic;
};

#define CTX_MAGIC 0x3465A4FD




typedef int func_t(int);

int try(struct ctx_s *pctx, func_t* f, int arg);

int throw(struct ctx_s *pctx, int r);
