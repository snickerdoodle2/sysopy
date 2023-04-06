#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
	int w;
	key_t klucz = ftok("./plik1", 'p');
	printf("%d\n", klucz);
	int id_kolejki = msgget(klucz, 0600);

	while(1){
		if (msgrcv(id_kolejki, &w, sizeof(int), 0, IPC_NOWAIT) < 0) break;
		printf("otrzymano: %d\n", w);
	}
	msgctl(id_kolejki, IPC_RMID, NULL);
	return 0;
}