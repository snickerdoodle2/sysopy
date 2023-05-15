#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int waiting_elfs, reindeers_home;
pthread_t reindeers[9];
pthread_t elfs[10];
pthread_t santa;

void * reindeer_handler() {
    printf("Reindeer\n");
    return NULL;
}

void * elf_handler() {
    printf("Elf\n");
    return NULL;
}

void * santa_handler() {
    printf("Santa\n");
    return NULL;
}

int main() {
    waiting_elfs = 0;
    reindeers_home = 0;

    for (int i = 0; i < 9; i++) {
        pthread_create(&reindeers[i], NULL, reindeer_handler, NULL);
    }

    for (int i = 0; i < 9; i++) {
        pthread_create(&elfs[i], NULL, elf_handler, NULL);
    }

    pthread_create(&santa, NULL, santa_handler, NULL);

    pthread_join(santa, NULL); // Wait until santa is finished
    return 0;
}
