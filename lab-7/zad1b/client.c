#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>
#include "sema.h"
#include "shared.h"
#include "lib.h"

char * lobby_shared;
char * move_shared;

Sema lobby_sema;
Sema seats_sema;

void create_shared() {
    lobby_shared = connect_shared(LOBBY_QUEUE_NAME, QUEUE_SIZE);
    move_shared = connect_shared(MOVE_QUEUE_NAME, QUEUE_SIZE);
}

void destroy_shared() {
    delete_shared(lobby_shared);
    delete_shared(move_shared);
}

void create_semas() {
    lobby_sema = open_sema(LOBBY_SEMA_NAME, LOBBY_CAP);
    seats_sema = open_sema(SEATS_SEMA_NAME, SEATS_AMOUNT);
}

int main() {
    srand(time(NULL) + getpid());

    create_shared();
    create_semas();

    printf("Klient o id: %d zaczal prace!\n", getpid());

    int found = 0;
    for (int i = 0; i < LOBBY_CAP; i++) {
        if (lobby_shared[i] == (char) 0) {
            found = 1;
            lobby_shared[i] = (char) (rand() % 8 + 1);
            wait_sema(lobby_sema, i);
            
            int seat_id = move_shared[i];
            lobby_shared[i] = (char) 0;
            wait_sema(seats_sema, seat_id);
            break;
        }
    }
    destroy_shared();

    if (!found) {
        printf("Nie ma miejsca w poczekalni\n");
    } else {
        printf("Klient o id: %d zakonczyl prace!\n", getpid());
    }

    return 0;
}
