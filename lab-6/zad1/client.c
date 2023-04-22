#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "types.h"

#include <unistd.h>
#include <errno.h>

int run = 1;
void handle(int signum) {
	printf("Koncze prace...\n");
	run = 0;
}

int main() {
	srand(time(NULL));

	char * homedir = getenv("HOME");
	key_t server_key = ftok(homedir, '0');
	key_t client_key = ftok(homedir, rand());
	int client_queue = msgget(client_key, IPC_CREAT | 0666);


	int server_queue = msgget(server_key, 0);
	if (server_queue == -1) {
		printf("Nie udalo sie otworzyc kolejki serwera!\n");
		return 1;
	}

	int res = 0;
	struct init_msg * init_msg = malloc(sizeof(struct init_msg));
	init_msg->msg_type = CLIENT_INIT;
	init_msg->IPC_ID = client_key;
	res = msgsnd(server_queue, init_msg, sizeof(init_msg->IPC_ID), 0);
	free(init_msg);

	signal(SIGINT, handle);
	while(run);

	struct command_msg command1;
	command1.msg_type = COMMAND_2ONE;
	command1.recipient_id = 1;
	strcpy(command1.msg, "Siemka :D\n");
	msgsnd(client_queue, &command1, sizeof(struct command_msg) - sizeof(long), 0);

	struct response_msg response;

	run = 1;

	while (run) {
		res = msgrcv(client_queue, &response, RESPONSE_LENGTH, 0, IPC_NOWAIT);
		if (res > 0) {
			printf("--------------------\n");
			printf("%s", response.res);
			printf("--------------------\n");
			fflush(stdout);
		}
	}

	struct command_msg command2;
	command2.msg_type = COMMAND_STOP;
	msgsnd(client_queue, &command2, sizeof(struct command_msg) - sizeof(long), 0);
	

	sleep(5);


	msgctl(client_queue, IPC_RMID, NULL);
	return 0;
}