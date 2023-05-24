#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


int main() {
    int status, gniazdo, dlugosc, nr=0, end=1, gniazdo2, l_bajtow;
    struct sockaddr_in ser,cli;
    char buf[200];

    gniazdo = socket(AF_INET, SOCK_DGRAM, 0);
    if (gniazdo == -1) return 1;

    memset(&ser, 0, sizeof(ser));

    ser.sin_family = AF_INET;
    ser.sin_port = htons(9001);
    ser.sin_addr.s_addr = htonl(INADDR_ANY);

    status = bind(gniazdo, (struct sockaddr *) &ser, sizeof(ser));
    if (status == -1) return 1;

    while(1) {
        dlugosc = sizeof(cli);
        l_bajtow = recvfrom(gniazdo, buf, sizeof(buf), 0, (struct sockaddr *) &cli, (socklen_t *) &dlugosc);
        buf[l_bajtow] = 0;
        printf("Otrzymano: %s\n", buf);
        fflush(stdout);

        if (l_bajtow > 0) {
            l_bajtow = sendto(gniazdo, "To jest odpowiedz serwera", 28, 0, (struct sockaddr *) &cli, dlugosc);
        }
    }
    close(gniazdo);
    return 0;
}
