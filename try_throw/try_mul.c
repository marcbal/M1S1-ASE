#include <stdio.h>
#include "try.h"

struct ctx_s ctx;


int mul(int depth) {
    int value;

    switch (scanf("%d", &value)) {
    case EOF:
        return 1; /* neutral element */
    case 0:
        return mul(depth+1); /* erroneous read */
    case 1:
        if (value)
            return value * mul(depth+1);
        else
            throw(&ctx, 0);
    }
    return 0;
}



int main(int argc, char** argv) {
    int ret = try(&ctx, mul, 0);
    printf("%d\n", ret);
}
