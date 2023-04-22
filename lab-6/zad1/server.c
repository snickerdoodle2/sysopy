#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include "types.h"

#include <unistd.h>
#include <errno.h>

char * list_clients(int * clients) {
	char * out = malloc(RESPONSE_LENGTH);
	sprintf(out, "Aktywni klienci:\n");
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i] != 0) {
			char * tmp = malloc(16);
			sprintf(tmp, "%d\n", i);
			strncat(out, tmp, strlen(tmp));
			free(tmp);
		}
	}

	return out;
}

char * format_message(char * msg_body, int author) {
	char * out = malloc(RESPONSE_LENGTH);
	sprintf(out, "%d: ", author);
	strncat(out, msg_body, strlen(msg_body));
	return out;
}

int run = 1;
void handle(int signum) {
	printf("Koncze prace...\n");
	run = 0;
}

int main() {
	char * homedir = getenv("HOME");
	key_t server_key = ftok(homedir, '0');
	printf("%d\n", server_key);
	int server_queue = msgget(server_key, IPC_CREAT | 0666);
	if (server_queue == -1) {
		printf("Nie udalo sie utworzyc kolejki!\n");
		return 1;
	}

	signal(SIGINT, handle);

	int * clients = calloc(MAX_CLIENTS, sizeof(int));
	int cur_client_id = 0;
	int active_clients = 0;

	struct init_msg init_msg;
	int msg_res;

	while(run || active_clients){
		if (run){
			msg_res = msgrcv(server_queue, &init_msg, sizeof(int), CLIENT_INIT, IPC_NOWAIT);
			if (msg_res != -1) {
				int client_queue = msgget(init_msg.IPC_ID, 0);
				clients[cur_client_id] = client_queue;
				active_clients++;
				printf("Klient nr %d dolaczyl! Aktywni: %d\n", cur_client_id, active_clients);
				fflush(stdout);
				cur_client_id++;
			}
		}

		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (clients[i] == 0) continue;
			int client_queue = clients[i];
			if (!run) {
				struct response_msg halt_msg;
				halt_msg.msg_type = SERVER_STOP;
				msgsnd(client_queue, &halt_msg, RESPONSE_LENGTH, 0);
			}
			struct command_msg command;
			msg_res = msgrcv(client_queue, &command, sizeof(command) - sizeof(long), -6, IPC_NOWAIT);
			if (msg_res < 0) continue;
			switch(command.msg_type) {
				case COMMAND_STOP:
					clients[i] = 0;
					active_clients--;
					printf("Klient nr %d nas opuscil :(. Aktywni: %d\n", i, active_clients);
					fflush(stdout);
					break;
				case COMMAND_LIST:
					sleep(0);
					struct response_msg res;
					res.msg_type = CLIENT_RESPONSE;
					char * list = list_clients(clients);
					strcpy(res.res, list);
					free(list);
					msgsnd(client_queue, &res, RESPONSE_LENGTH, 0);

					break;
				case COMMAND_2ALL:
					for (int j = 0; j < MAX_CLIENTS; j++) {
						if (clients[j] == 0 || j == i) continue;
						struct response_msg message;
						message.msg_type = CLIENT_RESPONSE;
						char * formatted_mesage = format_message(command.msg, i);
						strcpy(message.res, formatted_mesage);
						free(formatted_mesage);
						msg_res = msgsnd(clients[j], &message, RESPONSE_LENGTH, 0);
					}
					break;
				case COMMAND_2ONE:
					sleep(0);
					int recepient_queue = clients[command.recipient_id];
					if (recepient_queue != 0){
						struct response_msg message;
						message.msg_type = CLIENT_RESPONSE;
						char * formatted_mesage = format_message(command.msg, i);
						strcpy(message.res, formatted_mesage);
						free(formatted_mesage);
						msg_res = msgsnd(recepient_queue, &message, RESPONSE_LENGTH, 0);
					}
					break;
				default:
					break;
			}
		}
	}

	free(clients);
	msgctl(server_queue, IPC_RMID, NULL);
	return 0;
}
