#include <stdlib.h>
#include "shared.h"
#include <stdio.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

int make_shared(char * name, int size) {
    key_t key = ftok(getenv("HOME"), name[0]);
    if (key == -1) {
        perror("ftok");
        return -1;
    }
    return shmget(key, size, 0664 | IPC_CREAT);
}

char * connect_shared(char * name, int size) {
    int shared_id = make_shared(name, size);
    if (shared_id == -1) {
        perror("create_shared");
        return NULL; 
    }
    char * shared = shmat(shared_id, NULL, 0);

    if (shared == (char *)(-1)) {
        perror("connect_shared");
        return NULL;
    }
    return shared;
}

void disconnect_shared(char *shared) {
    shmdt(shared);
}

void delete_shared(char *name) {
    int shared_id = make_shared(name, 0);
    if (shared_id == -1) return;
    shmctl(shared_id, IPC_RMID, NULL);
}
