#include "shared.h"
#include "message.h"
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_CLIENTS 16

#define SOCKET_EVENT 0
#define CLIENT_EVENT 1

struct client {
    char nickname[128];
    bool active;
    bool taken;
    bool responding;
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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
            pthread_mutex_lock(&mutex);
            clients[i].taken = true;
            clients[i].active = false;
            clients[i].fd = client_fd;
            pthread_mutex_unlock(&mutex);
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

char * list_clients(void) {
	char * out = malloc(MSG_LEN);
	sprintf(out, "Aktywni klienci:\n");
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].active) {
			char * tmp = malloc(MSG_LEN + 3);
			sprintf(tmp, "%s\n", clients[i].nickname);
			strncat(out, tmp, strlen(tmp));
			free(tmp);
		}
	}

	return out;
}

void handle_client_message(struct client * client) {
    if (!client->active) {
        int bytes_read = read(client->fd, client->nickname, 127);
        client->nickname[bytes_read] = 0;
        client->active = true;
        client->responding = true;
        return;
    }

    struct message msg;
    read(client->fd, &msg, sizeof(msg));

    switch (msg.type) {
        case MSG_PING: {
            pthread_mutex_lock(&mutex);
            client->responding = true;
            pthread_mutex_unlock(&mutex);
            break;
        }
        case MSG_2ONE: {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].taken && strcmp(msg.recipient_nickname, clients[i].nickname) == 0) {
                    char buf[MSG_LEN * 2];
                    sprintf(buf, "%s: %s", client->nickname, msg.msg_body);
                    send_message(&clients[i], MSG_RESP, buf);
                }
            }
            break;
        }
        case MSG_2ALL: {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].taken && strcmp(client->nickname, clients[i].nickname) != 0) {
                    char buf[MSG_LEN * 2];
                    sprintf(buf, "%s: %s", client->nickname, msg.msg_body);
                    send_message(&clients[i], MSG_RESP, buf);
                }
            }
            break;
        }
        case MSG_LIST: {
            char * clients_list = list_clients();
            send_message(client, MSG_RESP, clients_list);
            free(clients_list);
            break;
        }
        case MSG_STOP: {
            pthread_mutex_lock(&mutex);
            epoll_ctl(epoll, EPOLL_CTL_DEL, client->fd, NULL);
            client->active = false;
            client->taken = false;
            pthread_mutex_unlock(&mutex);
            break;
        }
        default:
            break;

    }
}

void * ping(void * _) {
    while (1) {
        sleep(20);
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].taken == true) {
                printf("%d\n", clients[i].responding);
                fflush(stdout);
                if (clients[i].responding == true) {
                    struct message message;
                    message.type = MSG_PING;
                    clients[i].responding = false;
                    write(clients[i].fd, &message, sizeof(message));
                } else {
                    clients[i].responding = false;
                    epoll_ctl(epoll, EPOLL_CTL_DEL, clients[i].fd, NULL);
                    clients[i].taken = false;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
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

    pthread_t pinging_thread;
    pthread_create(&pinging_thread, NULL, ping, NULL);

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
