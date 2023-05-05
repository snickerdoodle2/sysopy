#include <stdlib.h>
#include "sema.h"
#include "shared.h"
#include "lib.h"

char * lobby_queue[LOBBY_CAP];
char * seats_queue[SEATS_AMOUNT];

Sema lobby_sema;
Sema seats_sema;

int main() {
}

void create_queues() {
    for (int i = 0; i < LOBBY_CAP; i++) {
        lobby_queue[i] = connect_shared(SEATS_SHARED_NAME, QUEUE_SIZE);
        if (lobby_queue[i] == NULL) {
            exit(1);
        }
    }

    for (int i = 0; i < SEATS_AMOUNT; i++) {
        seats_queue[i] = connect_shared(SEATS_SHARED_NAME, QUEUE_SIZE);
        if (seats_queue[i] == NULL) {
            exit(1);
        }
    }
}

void destroy_queues() {
    for (int i = 0; i < LOBBY_CAP; i++) {
        destroy_shared(lobby_queue[i]);
    }
    for (int i = 0; i < SEATS_AMOUNT; i++) {
        destroy_shared(seats_queue[i]);
    }
}

void create_semas() {
    lobby_sema = create_sema(LOBBY_SEMA_NAME, LOBBY_CAP, 0);
    seats_sema = create_sema(SEATS_SEMA_NAME, SEATS_AMOUNT, 0);
}

void destroy_semas() {
    destroy_sema(LOBBY_SEMA_NAME);
    destroy_sema(SEATS_SEMA_NAME);
}
