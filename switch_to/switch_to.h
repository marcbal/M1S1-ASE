




typedef void (func_t)(void*);

enum ctx_st_e {CTX_INIT, CTX_EXEC, CTX_END};

struct ctx_s {
    unsigned       ctx_magic;
    void*          ctx_esp;
    void*          ctx_ebp;
    func_t*        ctx_f;
    void*          ctx_arg;
    char*          ctx_stack;
    enum ctx_st_e  ctx_state;
};

#define CTX_MAGIC 0x3465A4FD


int init_ctx(struct ctx_s* ctx, int stack_size, func_t f, void* arg);

void switch_to_ctx(struct ctx_s* ctx);
