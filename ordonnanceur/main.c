#include "context.h"
#include <stdio.h>


void f_ping(void *arg);
void f_pong(void *arg);

int main(int argc, char *argv[])
{
	for(int i=0; i<100000; i++) {
		for (int j=0; j<50; j++) {
			create_ctx(1638400, f_ping, NULL);
			create_ctx(1638400, f_pong, NULL);
		}
		printf("Avant le yield\n");
		yield();
		printf("Retour au main\n");
	}

}

void f_ping(void *args)
{
    for(int i=0; i<2; i++) {
        printf("A");
		yield();
        printf("B");
        printf("C");
		yield();
    }
}

void f_pong(void *args)
{
    for(int i=0; i<2; i++) {
        printf("1");
        yield();
        printf("2");
        yield();
    }
}
