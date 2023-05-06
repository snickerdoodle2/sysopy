#include <stdlib.h>
#include "shared.h"
#include <stdio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int make_shared(char * name, int size) {
    int shared = shm_open(name, O_CREAT | O_RDWR, 0664);
    if (shared == -1) return -1;
    if (ftruncate(shared, size) == -1) {
        perror("ftruncate");
        return -1;
    }
    return shared;
}

char * connect_shared(char * name, int size) {
    int shared_id = make_shared(name, size);
    if (shared_id == -1) {
        perror("create_shared");
        return NULL; 
    }
    char * shared = (char *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_id, 0);
    return shared;
}

void disconnect_shared(char *shared, int size) {
    munmap(shared, size);
}

void delete_shared(char *name) {
    shm_unlink(name);
}
