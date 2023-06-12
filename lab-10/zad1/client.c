#include "shared.h"
#include "message.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

int sock;

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

void handle_stdin(void) {
    char buffer[512];
    char delims[] = " \t\n";

    int x = read(STDIN_FILENO, buffer, 512);
    buffer[x] = 0;

    struct message message;
    int send = 1;

    char * command = strtok(buffer, delims);

    if (strcmp(command, "LIST") == 0) {
        message.type = MSG_LIST;
    } else if (strcmp(command, "2ALL") == 0) {
        message.type = MSG_2ALL;
        char * msg_body = strtok(NULL, "\n");
        if (msg_body != NULL) strcpy(message.msg_body, msg_body);
        else send = 0;
    } else if (strcmp(command, "2ONE") == 0) {
        message.type = MSG_2ONE;
        char * recipient = strtok(NULL, delims);
        if (recipient != NULL) strcpy(message.recipient_nickname, recipient);
        else send = 0;

        char * msg_body = strtok(NULL, "\n");
        if (msg_body != NULL) strcpy(message.msg_body, msg_body);
        else send = 0;
    } else if (strcmp(command, "STOP") == 0) {
        raise(SIGINT);
    } else {
        send = 0;
        printf("Nieznana komenda!\n");
        fflush(stdout);
    }

    if (send) {
        write(sock, &message, sizeof(message));
        fflush(stdout);
    }
}

void signal_handler(int _) {
    close(sock);
    exit(0);
}

int main(int argc, char ** argv) {
#if 1
    if (argc == 5 && strcmp(argv[2], "web") == 0) sock = connect_web(argv[3], atoi(argv[4]));
    else if (argc == 4 && strcmp(argv[2], "file") == 0) sock = connect_file(argv[3]);
    else {
        printf("Użycie: ./client <nazwa> <web|file> <addr port|path>\n");
        return 1;
    }
#endif

    signal(SIGINT, signal_handler);
    int epoll = epoll_create1(0);

    struct epoll_event stdin_event; 
    stdin_event.events = EPOLLIN | EPOLLPRI;
    stdin_event.data.fd = STDIN_FILENO;

    epoll_ctl(epoll, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);
    
    struct epoll_event sock_event;
    sock_event.events = EPOLLIN;
    sock_event.data.fd = sock;

    epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &sock_event);

    char * nickname = argv[1];
    int _ = write(sock, nickname, strlen(nickname));

    struct epoll_event events[16];

    while(1) {
        int event_no = epoll_wait(epoll, events, 16, 1);
        for (int i = 0; i < event_no; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                handle_stdin();
            } else if (events[i].data.fd == sock) {
                struct message msg;
                read(sock, &msg, sizeof(msg));
                printf("%s\n", msg.msg_body);
                fflush(stdout);
            }
        }
    }


    return 0;
} 
