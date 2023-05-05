#include <stdio.h>
#include <unistd.h>
#include "lib.h"

int main() {
    Sema sema = open_sema("siema");
    printf("Przed\n");
    sleep(1);
    decrement(sema);
    printf("Po\n");
    return 0;
}
