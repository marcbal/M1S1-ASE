




typedef void (func_t)(void*);

enum ctx_st_e {CTX_INIT, CTX_EXEC, CTX_END, CTX_SEM};

typedef struct ctx_s {
    unsigned       ctx_magic;
    void*          ctx_esp;
    void*          ctx_ebp;
    func_t*        ctx_f;
    void*          ctx_arg;
    char*          ctx_stack;
    enum ctx_st_e  ctx_state;
    
    struct ctx_s*         next;
    
    struct ctx_s*         sem_next_ctx;
} ctx_s;


extern ctx_s* cctx;

#define CTX_MAGIC 0x3465A4FD


int create_ctx(int stack_size, func_t f, void* arg);

void yield();
