#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define ELF_AMOUNT 10
#define REINDEER_AMOUNT 9

int waiting_elfs, reindeers_home;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

pthread_t reindeers[REINDEER_AMOUNT];
pthread_t elfs[ELF_AMOUNT];
pthread_t santa;

void * reindeer_handler() {
    printf("Reindeer\n");
    while(1);
    return NULL;
}

void * elf_handler() {
    unsigned int state;
    state = time(NULL) ^ pthread_self();
    while (1) {
        int time_to_sleep = rand_r(&state) % 4 + 2;
        sleep(time_to_sleep);
        pthread_mutex_lock(&mutex);
        if (waiting_elfs < 3) {
            waiting_elfs++;
            printf("Elf: czeka %d elfow na Mikolaja (%ld)\n", waiting_elfs, pthread_self());
            fflush(stdout);
            if (waiting_elfs == 3) {
                // Wake up santa
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void * santa_handler() {
    printf("Santa\n");
    sleep(10);
    for (int i = 0; i < ELF_AMOUNT; i++) {
        pthread_cancel(elfs[i]);
    }


    for (int i = 0; i < REINDEER_AMOUNT; i++) {
        pthread_cancel(reindeers[i]);
    }
    return NULL;
}

int main() {
    waiting_elfs = 0;
    reindeers_home = 0;

    for (int i = 0; i < REINDEER_AMOUNT; i++) {
        pthread_create(&reindeers[i], NULL, reindeer_handler, NULL);
    }

    for (int i = 0; i < ELF_AMOUNT; i++) {
        pthread_create(&elfs[i], NULL, elf_handler, NULL);
    }

    pthread_create(&santa, NULL, santa_handler, NULL);

    pthread_join(santa, NULL); // Wait until santa is finished
    return 0;
}
