#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
	int i;
	key_t klucz = ftok("./plik1", 'p');
	printf("%d\n", klucz);
	int id_kolejki = msgget(klucz, IPC_CREAT | 0600);
	for (i = 0; i < 5; i++) msgsnd(id_kolejki, &i, sizeof(int), 0);
	return 0;
}