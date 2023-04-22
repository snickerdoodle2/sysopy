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
int client_queue;
int pid;
void handle(int signum) {
	if (pid != 0) {
		kill(pid, SIGINT);
		struct command_msg command2;
		command2.msg_type = COMMAND_STOP;
		msgsnd(client_queue, &command2, sizeof(struct command_msg) - sizeof(long), 0);
	}
	// czekaj na odpowiedz serwera
	sleep(1);
	msgctl(client_queue, IPC_RMID, NULL);
	exit(0);
}

int main() {
	srand(time(NULL));

	char * homedir = getenv("HOME");
	key_t server_key = ftok(homedir, '0');
	key_t client_key = ftok(homedir, rand());
	client_queue = msgget(client_key, IPC_CREAT | 0666);


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

	pid = fork();

	if (pid == 0) {
		struct response_msg response;
		while (run) {
			res = msgrcv(client_queue, &response, RESPONSE_LENGTH, 0, IPC_NOWAIT);
			if (res > 0) {
				if (response.msg_type == SERVER_STOP) {
					kill(getppid(), SIGINT);
				} else {
					printf("%s", response.res);
					fflush(stdout);
				}
			}
		}
		return 0;
	} else {
		char command_buffer[1024];
		fflush(stdout);
		while(run && fgets(command_buffer, 1024, stdin)) {
			char * command = strtok(command_buffer, " ");

			if (strcmp(command, "LIST\n") == 0) {
				struct command_msg command;
				command.msg_type = COMMAND_LIST;
				msgsnd(client_queue, &command, sizeof(struct command_msg) - sizeof(long), 0);
			} else if (strcmp(command, "2ALL") == 0) {
				struct command_msg command;
				command.msg_type = COMMAND_2ALL;
				char * message = strtok(NULL, "");
				if (message != NULL) {
					strcpy(command.msg, message);
					msgsnd(client_queue, &command, sizeof(struct command_msg) - sizeof(long), 0);
				}
			} else if (strcmp(command, "2ONE") == 0) {
				struct command_msg command;
				command.msg_type = COMMAND_2ONE;
				command.recipient_id = atoi(strtok(NULL, " "));
				char * message = strtok(NULL, "");
				if (message != NULL) {
					strcpy(command.msg, message);
					msgsnd(client_queue, &command, sizeof(struct command_msg) - sizeof(long), 0);
				}
			} else if (strcmp(command, "STOP\n") == 0) {
				run = 0;
			} else {
				printf("Nieznana komenda!\n");
				fflush(stdout);
			}
		}
	}
	return 0;
}
