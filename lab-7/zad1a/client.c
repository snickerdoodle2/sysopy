#include <stdio.h>
#include <unistd.h>
#include "sema.h"

int main() {
    Sema sema = open_sema("siema");
    printf("Przed\n");
    sleep(1);
    decrement(sema, 0);
    printf("Po\n");
    return 0;
}
