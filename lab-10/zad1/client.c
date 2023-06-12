#include "shared.h"
#include "message.h"
#include <signal.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
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

int create_web(void) {
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(0);
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
        printf("Nieprawidlowy adres.\n");
        exit(1);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("connect");
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
        printf("MSG_TYPE: %d\n", message.type);
        fflush(stdout);
    }
}

int client_sock;
int server_sock;
void signal_handler(int _) {
    close(client_sock);
    close(server_sock);
    exit(0);
}

int main(int argc, char ** argv) {
#if 1
    if (argc == 5 && strcmp(argv[2], "web") == 0) server_sock = connect_web(argv[3], atoi(argv[4]));
    else if (argc == 4 && strcmp(argv[2], "file") == 0) server_sock = connect_file(argv[3]);
    else {
        printf("Użycie: ./client <nazwa> <web|file> <addr port|path>\n");
        return 1;
    }
#endif

    signal(SIGINT, signal_handler);
    client_sock = create_web();
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    if (getsockname(client_sock, (struct sockaddr *) &client_addr, &len) == -1) return 1;
    printf("Client port: %d\n", client_addr.sin_port);

    int epoll = epoll_create1(0);

    struct epoll_event stdin_event; 
    stdin_event.events = EPOLLIN | EPOLLPRI;
    stdin_event.data.fd = STDIN_FILENO;

    epoll_ctl(epoll, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);
    
    struct epoll_event client_sock_event;
    client_sock_event.events = EPOLLIN;
    stdin_event.data.fd = client_sock;

    epoll_ctl(epoll, EPOLL_CTL_ADD, client_sock, &client_sock_event);

    struct epoll_event events[16];

    while(1) {
        int event_no = epoll_wait(epoll, events, 16, 1);
        for (int i = 0; i < event_no; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                handle_stdin();
            } else if (events[i].data.fd == client_sock) {
                printf("Wiadomosc nowa\n");
                fflush(stdout);
            }
        }
    }


    return 0;
} 
