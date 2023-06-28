#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "common.h"

int running = 1;

void handle_sigint(int _) {
    running = 0;
}

int main(void) {
    struct sockaddr_in server, client;
    int res;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Server socket init");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    res = bind(server_socket, (struct sockaddr *) &server, sizeof(server));
    if (res == -1) {
        perror("Bind");
        return 1;
    }

    res = listen(server_socket, 10);
    if (res == -1) {
        perror("Listen");
        return 1;
    }

    signal(SIGINT, handle_sigint);

    int client_len = sizeof(client);
    int req_len;
    char buffer[256];

    struct response res_data;

    while (running) {
        int client_socket = accept(server_socket, (struct sockaddr *) &client, (socklen_t *) &client_len);
        if (client_socket == -1) {
            perror("Client socket init");
            return 1;
        }

        req_len = read(client_socket, buffer, sizeof(buffer));
        buffer[req_len] = 0;
        printf("Klient chce plik: %s\n", buffer);
        FILE * f = fopen(buffer, "r");
        if (f == NULL) {
            res_data.code = RES_ERROR;
            res_data.packets = -1;
            write(client_socket, &res_data, sizeof(res_data));
            goto end_connection;
        }
        
        fseek(f, 0, SEEK_END);
        long f_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        res_data.code = RES_SUCCESS;
        res_data.packets = (int) ( (float) f_size / PACKET_SIZE) + 1;
        write(client_socket, &res_data, sizeof(res_data));

        void * buffer = malloc(PACKET_SIZE);
        int bytes_read;
        while ((bytes_read = fread(buffer, 1, PACKET_SIZE, f)) > 0) {
            write(client_socket, buffer, bytes_read);
        }
        free(buffer);

end_connection:
        close(client_socket);
    }

    close(server_socket);
    printf("Koniec pracy serwera...\n");
    fflush(stdout);
    return 0;
}
