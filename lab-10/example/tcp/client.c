#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

int main() {
    int status, gniazdo;
    struct sockaddr_in ser;
    char buf[200];

    gniazdo = socket(AF_INET, SOCK_STREAM, 0);
    if (gniazdo == -1) return 1;
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(9000);
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");

    status = connect(gniazdo, (struct sockaddr*) &ser, sizeof ser);
    if (status < 0) return 1;
    printf("Podaj tekst:\n");
    fgets(buf, sizeof(buf), stdin);
    status = write(gniazdo, buf, strlen(buf));
    status = read(gniazdo, buf, (sizeof buf) - 1);
    buf[status] = '\0';
    printf("Otrzymalem: %s\n", buf);

    close(gniazdo);
    printf("Koniec klienta\n");
    return 0;
}
