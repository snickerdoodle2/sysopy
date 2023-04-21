#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include <errno.h>

int main() {
	srand(time(NULL));

	char * homedir = getenv("HOME");
	key_t server_key = ftok(homedir, '0');
	key_t client_key = ftok(homedir, rand());
	printf("%d %d\n", server_key, client_key);

	int server_queue = msgget(server_key, 0);
	if (server_queue == -1) {
		printf("Nie udalo sie otworzyc kolejki serwera!\n");
		return 1;
	}

	struct init_msg * init_msg = malloc(sizeof(struct init_msg));
	init_msg->msg_type = CLIENT_INIT;
	init_msg->IPC_ID = client_key;
	int res = msgsnd(server_queue, init_msg, sizeof(init_msg->IPC_ID), 0);
	printf("%d %d\n", res, errno);
	free(init_msg);

	return 0;
}
