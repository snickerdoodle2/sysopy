#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <errno.h>
#include "sema.h"

Sema create_sema(char *name, int amount, int initial_value) {
    key_t key = ftok(getenv("HOME"), name[0]);
    if (key == -1) {
        perror("ftok: ");
        return 0;
    }

    Sema sema_id = semget(key, amount, 0664 | IPC_CREAT);
    if (sema_id == -1) {
        perror("semget: ");
        return 0;
    }

    for (int i = 0; i < amount; i++) {
        if (semctl(sema_id, i, SETVAL, initial_value) == -1) {
            perror("semctl: ");
            return 0;
        }
    }

    return sema_id;
}

Sema open_sema(char *name, int amount) {
    key_t key = ftok(getenv("HOME"), name[0]);
    if (key == -1) {
        perror("ftok: ");
        return 0;
    }

    Sema sema_id = semget(key, amount, 0);
    if (sema_id == -1) {
        return 0;
    }
    return sema_id;
}

void close_sema(Sema sema_id) {
    return;
}

void destroy_sema(char *name, int amount) {
    Sema sema_id = open_sema(name, amount);
    semctl(sema_id, 0, IPC_RMID);
}

void increment(Sema sema_id, int room_id){
    struct sembuf ops = { room_id, 1, 0 };
    if (semop(sema_id, &ops, 1)) {
        perror("increment");
    }
}

void wait_sema(Sema sema_id, int room_id) {
    struct sembuf ops = { room_id, 0, 0 };
    if (semop(sema_id, &ops, 1)) {
        printf("%d\n", room_id);
        perror("wait");
    }
}

void decrement(Sema sema_id, int room_id){
    struct sembuf ops = { room_id, -1, 0 };
    if (semop(sema_id, &ops, 1)) {
        perror("increment");
    }
}
