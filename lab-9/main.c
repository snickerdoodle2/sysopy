#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ELF_AMOUNT 10
#define REINDEER_AMOUNT 9

int waiting_elfs, reindeers_home, elfs_wait, reindeers_wait;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

pthread_cond_t elfs_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeers_cond = PTHREAD_COND_INITIALIZER;

pthread_t waiting_elfs_tab[3];

pthread_t reindeers[REINDEER_AMOUNT];
pthread_t elfs[ELF_AMOUNT];
pthread_t santa;

void * reindeer_handler() {
    unsigned int state;
    state = time(NULL) ^ pthread_self();
    while (1) {
        int time_to_sleep = rand_r(&state) % 6 + 5;
        sleep(time_to_sleep);

        pthread_mutex_lock(&mutex);
        if (reindeers_home == 0) reindeers_wait = 1;
        reindeers_home++;
        printf("Renifer: czeka %d reniferow na Mikolaja (%ld)\n", reindeers_home, pthread_self());
        fflush(stdout);
        if (reindeers_home == REINDEER_AMOUNT) {
            printf("Renifer: wybudzam Mikolaja (%ld)\n", pthread_self());
            fflush(stdout);
            pthread_cond_broadcast(&condition);
        }
        while(reindeers_wait){
            pthread_cond_wait(&reindeers_cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void sighandler(){}

void * elf_handler() {
    signal(SIGUSR1, sighandler);
    unsigned int state;
    state = time(NULL) ^ pthread_self();
    while (1) {
        int time_to_sleep = rand_r(&state) % 4 + 2;
        sleep(time_to_sleep);

        pthread_mutex_lock(&mutex);
        int locked_waiting = waiting_elfs;
        pthread_mutex_unlock(&mutex);
        
        if (locked_waiting < 3) {
            pthread_mutex_lock(&mutex);
            if (waiting_elfs == 0) elfs_wait = 1;
            waiting_elfs_tab[waiting_elfs] = pthread_self();
            waiting_elfs++;
            printf("Elf: czeka %d elfow na Mikolaja (%ld)\n", waiting_elfs, pthread_self());
            fflush(stdout);
            if (waiting_elfs == 3) {
                printf("Elf: wybudzam Mikolaja (%ld)\n", pthread_self());
                fflush(stdout);
                pthread_cond_broadcast(&condition);
            }
            pthread_mutex_unlock(&mutex);
            pause();
            printf("Elf: Mikolaj rozwiazal moj problem! (%ld)\n", pthread_self());
            fflush(stdout);
        } else {
            printf("Elf: samodzielnie rozwiazuje problem (%ld)\n", pthread_self());
            fflush(stdout);
        }
    }
    return NULL;
}

void * santa_handler() {
    int gifts_amount = 0;
    unsigned int state;
    state = time(NULL) ^ pthread_self();

    while (gifts_amount < 3) {
        printf("Mikolaj: Zasypiam\n");
        fflush(stdout);
        pthread_mutex_lock(&mutex);
        while(reindeers_home < 9 && waiting_elfs < 3) {
            pthread_cond_wait(&condition, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        printf("Mikolaj: Budze sie\n");
        fflush(stdout);

        if (waiting_elfs == 3) {
            printf("Mikolaj: rozwiazuje problemy elfow: %ld, %ld, %ld\n", waiting_elfs_tab[0], waiting_elfs_tab[1], waiting_elfs_tab[2]);
            fflush(stdout);
            pthread_mutex_lock(&mutex);
            elfs_wait = 0;
            pthread_mutex_unlock(&mutex);
            for (int i = 0; i < 3; i++){
                int repair_time = rand_r(&state) % 2 + 1;
                sleep(repair_time);
                pthread_kill(waiting_elfs_tab[i], SIGUSR1);
            }
            pthread_mutex_lock(&mutex);
            waiting_elfs = 0;
            pthread_mutex_unlock(&mutex);
        }

        if (reindeers_home == 9) {
            printf("Mikolaj: dostarczam zabawki\n");
            fflush(stdout);
            int gift_time = rand_r(&state) % 3 + 2;
            sleep(gift_time);
            gifts_amount++;
            pthread_mutex_lock(&mutex);
            reindeers_wait = 0;
            reindeers_home = 0;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&reindeers_cond);
        }
    }
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

    elfs_wait = 0;
    reindeers_wait = 0;

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
