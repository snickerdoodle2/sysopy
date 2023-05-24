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

    l_bajtow = sendto(gniazdo, "to jest zapytanie od klienta", 28, 0, (struct sockaddr *) &ser, sizeof(ser));
    dlugosc = sizeof(cli);
    l_bajtow = recvfrom(gniazdo, buf, sizeof(buf), 0, NULL, NULL);
    if (l_bajtow > 0) {
        buf[l_bajtow] = 0;
        printf("Otrzymano: %s\n", buf);
        fflush(stdout);
    }
    close(gniazdo);
    return 0;
}
