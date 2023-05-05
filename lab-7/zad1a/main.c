#include "sema.h"
#include <stdio.h>

int main() {
    destroy_sema("siema");
    Sema sema = create_sema("siema", 1, 0);
    printf("Przed\n");
    increment(sema, 0);
    wait(sema, 0);
    printf("Po\n");
    return 0;
}
