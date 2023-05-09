#include <stdio.h>
#include <pthread.h>

int waiting_elfs, reindeers_home;

void * reindeer_handler() {
    return NULL;
}

void * elf_handler() {
    return NULL;
}

void * santa_handler() {
    return NULL;
}

int main() {
    waiting_elfs = 0;
    reindeers_home = 0;

    pthread_t reindeers[9];
    pthread_t elfs[10];
    pthread_t santa;

    for (int i = 0; i < 9; i++) {
        pthread_create(&reindeers[i], NULL, reindeer_handler, NULL);
    }

    for (int i = 0; i < 9; i++) {
        pthread_create(&elfs[i], NULL, elf_handler, NULL);
    }

    pthread_create(&santa, NULL, santa_handler, NULL);
    

    return 0;
}
