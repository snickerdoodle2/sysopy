#include "shared.h"
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

int connect_web(char * ip_addr, int port) {
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_addr, &address.sin_addr) <= 0) {
        printf("Nieprawidlowy adres.\n");
        exit(1);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
        printf("Nie udalo sie polaczyc.\n");
        exit(1);
    }

    return sock;
}

int connect_file(char * path) {
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, path, 107);

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
        printf("Nie udalo sie polaczyc.\n");
        exit(1);
    }

    return sock;
}

int sock;
void signal_handler(int _) {
}

int main(int argc, char ** argv) {
#if 0
    if (argc == 5 && strcmp(argv[2], "web") == 0) sock = connect_web(argv[3], atoi(argv[4]));
    else if (argc == 4 && strcmp(argv[2], "file") == 0) sock = connect_file(argv[3]);
    else {
        printf("Użycie: ./client <nazwa> <web|file> <addr port|path>\n");
        return 1;
    }
#endif

    int epoll = epoll_create1(0);

    struct epoll_event stdin_event; 
    stdin_event.events = EPOLLIN | EPOLLPRI;
    stdin_event.data.fd = STDIN_FILENO;

    epoll_ctl(epoll, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

    struct epoll_event events[16];

    while(1) {
        int event_no = epoll_wait(epoll, events, 16, 1);
        for (int i = 0; i < event_no; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                printf("Got stdin\n");
                fflush(stdout);
            }
        }
    }

    return 0;
} 
