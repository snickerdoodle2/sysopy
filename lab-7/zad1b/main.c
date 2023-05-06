#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "sema.h"
#include "shared.h"
#include "lib.h"

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
    lobby_sema = create_sema(LOBBY_SEMA_NAME, LOBBY_CAP, 0);
    seats_sema = create_sema(SEATS_SEMA_NAME, SEATS_AMOUNT, 0);
}

void destroy_semas() {
    destroy_sema(LOBBY_SEMA_NAME, LOBBY_CAP);
    destroy_sema(SEATS_SEMA_NAME, SEATS_AMOUNT);
}

int main() {
    create_shared();
    for (int i = 0; i < LOBBY_CAP; i++) {
        lobby_shared[i] = (char) 0;
        reserved_shared[i] = (char) 0;
    }
    lobby_shared[LOBBY_CAP] = '\0';
    reserved_shared[LOBBY_CAP] = '\0';

    for (int i = 0; i < SEATS_AMOUNT; i++) {
        seats_shared[i] = (char) 0;
    }
    seats_shared[SEATS_AMOUNT] = '\0';


    destroy_semas();
    create_semas();

    // spawn barbers
    for (int i = 0; i < BARBER_AMOUNT; i++) {
        if (fork() == 0) execl(BARBER_EXE, BARBER_EXE, NULL);
        // else usleep(500 * 1000);
    }

    // spawn clients 
    for (int i = 0; i < CLIENT_AMOUNT; i++) {
        if (fork() == 0) execl(CLIENT_EXE, CLIENT_EXE, NULL);
        else usleep(200 * 1000);
    }

    // wait for everyone to stop running
    while(wait(NULL) > 0);

    destroy_shared();
    destroy_semas();
}
