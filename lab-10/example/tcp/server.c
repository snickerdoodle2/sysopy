#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>


int main() {
    int status, gniazdo, dlugosc, nr=0, end=1, gniazdo2;

    struct sockaddr_in ser, cli;
    char buf[200];

    gniazdo = socket(AF_INET, SOCK_STREAM, 0);
    if (gniazdo == -1) return 1;

    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(9000);
    ser.sin_addr.s_addr = htonl(INADDR_ANY);
    status = bind(gniazdo, (struct sockaddr *) &ser, sizeof(ser));
    if (status == -1) return 1;


    status = listen(gniazdo, 10);
    if (status == -1) return 1;

    while (end) {
        dlugosc = sizeof(cli);
        gniazdo2 = accept(gniazdo, (struct sockaddr *) &cli, (socklen_t *) &dlugosc);
        if (gniazdo2 == -1) return 1;

        read(gniazdo2, buf, sizeof(buf));
        if (buf[0] == 'Q') {
            sprintf(buf, "Serwer konczy prace");
            end = 0;
        }
        else if (buf[0] == 'N') sprintf(buf, "Jestes klientem nr %d", nr++);
        else sprintf(buf, "Nie rozumiem");

        write(gniazdo2, buf, strlen(buf));
        close(gniazdo2);
    }

    close(gniazdo);
    printf("Koniec serwera\n");
    fflush(stdout);

    return 0;
}
