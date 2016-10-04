#include "context.h"
#include <stdio.h>


void f_ping(void *arg);
void f_pong(void *arg);

int main(int argc, char *argv[])
{
    create_ctx(1638400, f_ping, NULL);
    create_ctx(1638400, f_pong, NULL);
    printf("Avant le yield\n");
    yield();
    printf("Retour au main\n");

}

void f_ping(void *args)
{
    for(int i=0; i<2; i++) {
        printf("A\n");
		yield();
        printf("B\n");
        printf("C\n");
		yield();
    }
}

void f_pong(void *args)
{
    for(int i=0; i<2; i++) {
        printf("1\n");
        yield();
        printf("2\n");
        yield();
    }
}
