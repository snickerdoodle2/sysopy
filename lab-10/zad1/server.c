#include "shared.h"
#include "message.h"
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_CLIENTS 16

#define SOCKET_EVENT 0
#define CLIENT_EVENT 1

struct client {
    char nickname[128];
    bool active;
    bool taken;
    int fd;
};

struct client clients[MAX_CLIENTS];

struct event_data {
    int type;
    union payload {
        struct client * client;
        int socket;
    } payload;
};

int epoll;

void init_socket(int socket, void * addr, int addr_size) {
    if (bind(socket, (struct sockaddr *) addr, addr_size) < 0) {
        perror("bind");
        exit(1);
    }
    if (listen(socket, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(1);
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    struct event_data * event_data = malloc(sizeof(struct event_data *));
    event_data->type = SOCKET_EVENT;
    event_data->payload.socket = socket;
    
    event.data.ptr = event_data;
    epoll_ctl(epoll, EPOLL_CTL_ADD, socket, &event);
}

struct client * init_client(int client_fd){
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].taken) {
            clients[i].taken = true;
            clients[i].active = false;
            clients[i].fd = client_fd;
            return &clients[i];
        }
    }
    return NULL;
}

void send_message(struct client * client, int msg_type, char * msg_body) {
    struct message res;
    res.type = msg_type;
    strcpy(res.msg_body, msg_body);
    write(client->fd, &res, sizeof(res));
}

void handle_client_message(struct client * client) {
    if (!client->active) {
        int bytes_read = read(client->fd, client->nickname, 127);
        client->nickname[bytes_read] = 0;
        client->active = true;
        return;
    }

    struct message msg;
    read(client->fd, &msg, sizeof(msg));

    switch (msg.type) {
        case MSG_2ALL:
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].taken) {
                    printf("wysylam\n");
                    fflush(stdout);
                    send_message(client, MSG_RESP, msg.msg_body);
                }
            }
            break;
    }
}

int main(int argc, char ** argv) {
    if (argc != 3) {
        printf("Usage: ./server [port] [path]\n");
        return 1;
    }
    int port = atoi(argv[1]);
    char * path = argv[2];

    epoll = epoll_create1(0);

    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path, path);

    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &web_addr.sin_addr) <= 0) {
        printf("Nieprawidlowy adres.\n");
        exit(1);
    }

    unlink(path);
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    init_socket(local_socket, &local_addr, sizeof(local_addr));

    int web_socket = socket(AF_INET, SOCK_STREAM, 0);
    init_socket(web_socket, &web_addr, sizeof(web_addr));

    // todo: ping

    printf("Server listening on: 127.0.0.1:%d and %s\n", port, path);
    struct epoll_event events[16];
    while (1) {
        int msg_waiting = epoll_wait(epoll, events, 16, -1);
        for (int i = 0; i < msg_waiting; i++) {
            struct event_data * data = events[i].data.ptr;
            if (data->type == SOCKET_EVENT) {
                int client_fd = accept(data->payload.socket, NULL, NULL);
                struct client * client = init_client(client_fd);
                if (client == NULL) {
                    struct message msg;
                    msg.type = MSG_FULL;
                    write(client_fd, &msg, sizeof(msg));
                    continue;
                }

                struct event_data * event_data = malloc(sizeof(struct event_data *));
                event_data->type = CLIENT_EVENT;
                event_data->payload.client = client;
                struct epoll_event event;
                event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
                event.data.ptr = event_data;
                epoll_ctl(epoll, EPOLL_CTL_ADD, client_fd, &event);
            } else if (data->type == CLIENT_EVENT) {
                handle_client_message(data->payload.client);
            }
        }
    }
    return 0;
}
