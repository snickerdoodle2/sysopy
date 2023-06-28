#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#define SERVER_ADDRESS "192.168.91.13"

int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("podaj nazwe pliku\n");
        return 1;
    }
    const char * file_name = argv[1];

    int res;
    struct sockaddr_in server;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Server socket init");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    res = connect(server_socket, (struct sockaddr *) &server, sizeof(server));
    if (res == -1) {
        perror("Connect");
        return 1;
    }


    struct response res_data;

    res = write(server_socket, file_name, strlen(file_name));
    res = read(server_socket, &res_data, sizeof(res_data));
    if (res_data.code == RES_ERROR) goto end_client;
    FILE * f = fopen(file_name, "w");
    if (f == NULL) {
        perror("fopen");
        goto end_client;
    }
    void * buffer = malloc(PACKET_SIZE);

    for (int i = 0; i < res_data.packets; i++) {
        res = read(server_socket, buffer, PACKET_SIZE);
        if (res == 0) goto close_file;
        fwrite(buffer, 1, res, f);
    }

close_file:
    fclose(f);
    free(buffer);

end_client:
    close(server_socket);
    printf("Koniec klienta\n");
    return 0;
}
