#include "lib.h"
#include <stdio.h>

int main() {
    destroy_sema("siema");
    Sema sema = create_sema("siema", 0);
    printf("Przed\n");
    increment(sema);
    wait(sema);
    printf("Po\n");
    return 0;
}
