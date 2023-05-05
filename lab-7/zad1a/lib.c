#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <errno.h>
#include "lib.h"

Sema create_sema(char *name, int initial_value) {
    key_t key = ftok(getenv("HOME"), name[0]);
    if (key == -1) {
        perror("ftok: ");
        return 0;
    }

    Sema sema_id = semget(key, 1, 0664 | IPC_CREAT);
    if (sema_id == -1) {
        perror("semget: ");
        return 0;
    }

    if (semctl(sema_id, 0, SETVAL, initial_value) == -1) {
        perror("semctl: ");
        return 0;
    }

    return sema_id;
}

Sema open_sema(char *name) {
    key_t key = ftok(getenv("HOME"), name[0]);
    if (key == -1) {
        perror("ftok: ");
        return 0;
    }

    Sema sema_id = semget(key, 1, 0);
    if (sema_id == -1) {
        perror("semget: ");
        return 0;
    }
    return sema_id;
}

void close_sema(Sema sema_id) {
    return;
}

void destroy_sema(char *name) {
    Sema sema_id = open_sema(name);
    semctl(sema_id, 0, IPC_RMID);
}
