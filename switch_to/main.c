#include "switch_to.h"
#include <stdio.h>

struct ctx_s ctx_ping;
struct ctx_s ctx_pong;

void f_ping(void *arg);
void f_pong(void *arg);

int main(int argc, char *argv[])
{
    init_ctx(&ctx_ping, 16384, f_ping, NULL);
    init_ctx(&ctx_pong, 16384, f_pong, NULL);
    switch_to_ctx(&ctx_ping);
    printf("Retour au main\n");

}

void f_ping(void *args)
{
    for (int i = 0; i < 100; i++) {
        printf("A") ;
        switch_to_ctx(&ctx_pong);
        printf("B") ;
        switch_to_ctx(&ctx_pong);
        printf("C") ;
        switch_to_ctx(&ctx_pong);
    }
}

void f_pong(void *args)
{
    while(1) {
        printf("1") ;
        switch_to_ctx(&ctx_ping);
        printf("2") ;
        switch_to_ctx(&ctx_ping);
    }
}
