#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include "sema.h"

Sema create_sema(char *name, int amount, int initial_value) {
    Sema sema = malloc(amount * sizeof(sem_t *));
    for (int i = 0; i < amount; i++) {
        char * tmp = malloc(256);
        sprintf(tmp, "%s%d", name, amount);
        sema[i] = sem_open(tmp, O_CREAT | O_EXCL, 0644, initial_value); 
        if (sema[i] == SEM_FAILED) {
            perror("create_sema");
            return 0;
        }
        free(tmp);
    }
    return sema;
}

Sema open_sema(char *name, int amount) {
    Sema sema = malloc(amount * sizeof(sem_t *));
    for (int i = 0; i < amount; i++) {
        char * tmp = malloc(256);
        sprintf(tmp, "%s%d", name, amount);
        sema[i] = sem_open(tmp, 0); 
        if (sema[i] == SEM_FAILED) {
            perror("open-sema");
            return 0;
        }
        free(tmp);
    }
    return sema;
}

void close_sema(Sema sema_id, int amount) {
    for (int i = 0; i < amount; i++) {
        sem_close(sema_id[i]);
    }
    free(sema_id);
    return;
}

void destroy_sema(char *name, int amount) {
    for (int i = 0; i < amount; i++) {
        char * tmp = malloc(256);
        sprintf(tmp, "%s%d", name, amount);
        sem_unlink(tmp);
        free(tmp);
    }
}

void increment(Sema sema_id, int room_id){
   sem_post(sema_id[room_id]); 
}

void wait_sema(Sema sema_id, int room_id) {
    sem_wait(sema_id[room_id]);
}
