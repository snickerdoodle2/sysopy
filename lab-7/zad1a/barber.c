#include "lib.h"
#include "sema.h"
#include "shared.h"
#include <stdio.h>
#include <unistd.h>

char * lobby_shared;
char * seats_shared;
char * move_shared;
char * reserved_shared;

Sema lobby_sema;
Sema seats_sema;

void create_shared() {
    lobby_shared = connect_shared(LOBBY_QUEUE_NAME, QUEUE_SIZE);
    seats_shared = connect_shared(SEATS_QUEUE_NAME, QUEUE_SIZE);
    move_shared = connect_shared(MOVE_QUEUE_NAME, QUEUE_SIZE);
    reserved_shared = connect_shared(MOVE_QUEUE_NAME, QUEUE_SIZE);
}

void destroy_shared() {
    delete_shared(lobby_shared);
    delete_shared(seats_shared);
    delete_shared(move_shared);
    delete_shared(reserved_shared);
}

void create_semas() {
    lobby_sema = open_sema(LOBBY_SEMA_NAME, LOBBY_CAP);
    seats_sema = open_sema(SEATS_SEMA_NAME, SEATS_AMOUNT);
}

int not_found = 0;

int main() {
    create_shared();
    create_semas();

    printf("Barber o id: %d zaczal prace!\n", getpid());

    while (1) {
        int found = 0;
        for (int i = 0; i < LOBBY_CAP; i++) {
            if (lobby_shared[i] != (char) 0 && reserved_shared[i] == (char) 0) {
                reserved_shared[i] = (char) 1;
                found = 1;
                int haircut = lobby_shared[i];
                
                for (int j = 0; j < SEATS_AMOUNT; j++) {
                    if (seats_shared[j] == (char) 0) {
                        seats_shared[j] = (char) 1;

                        increment(seats_sema, j);
                        move_shared[i] = (char) (j);
                        decrement(lobby_sema, i);
                        sleep(haircut);
                        decrement(seats_sema, j);
                        seats_shared[j] = (char) 0;
                        break;
                    }
                }
                reserved_shared[i] = (char) 0;

            }
        }
        if (!found) ++not_found;
        if (not_found >= 5) break;
        sleep(1);
    }

    destroy_shared();
    printf("Barber o id: %d zakonczyl prace!\n", getpid());

    return 0;
}
