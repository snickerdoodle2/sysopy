#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include "types.h"

int run = 1;
void handle(int signum) {
	printf("Koncze prace...\n");
	run = 0;
}

int main() {
	char * homedir = getenv("HOME");
	key_t server_key = ftok(homedir, '0');
	printf("%d\n", server_key);
	int res = msgget(server_key, IPC_CREAT | 0666);
	if (res == -1) {
		printf("Nie udalo sie utworzyc kolejki!\n");
		return 1;
	}

	signal(SIGINT, handle);

	int * clients = calloc(MAX_CLIENTS, sizeof(int));
	int cur_client = 0;

	struct init_msg init_msg;
	int msg_res;

	while(run){
		msg_res = msgrcv(server_key, &init_msg, sizeof(int), CLIENT_INIT, IPC_NOWAIT | S_IRUSR | S_IWUSR);
		if (msg_res != -1) {
			printf("Id klienta: %d\n", init_msg.IPC_ID);
		}
	}

	free(clients);
	msgctl(server_key, IPC_RMID, NULL);
	return 0;
}
