#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "types.h"

int run = 1;
void handle(int signum) {
	run = 0;
}

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


char * get_time() {
	time_t cur_time;
	struct tm * cur_time_info;

	time(&cur_time);
	cur_time_info = localtime(&cur_time);
	return asctime(cur_time_info);
}



int main()
{
    mqd_t mq_server;
    struct mq_attr attr;

    // Set up the attribute structure
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSG_NUM;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create the server message queue
	mq_unlink(QUEUE_NAME);
    mq_server = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
    if(mq_server == (mqd_t) -1)
    {
        perror("Server: mq_open()");
        exit(1);
    }

	FILE * logs = fopen("logs.txt", "w");
	if (logs == NULL) return 1;

	signal(SIGINT, handle);

	mqd_t * clients = calloc(MAX_CLIENTS, sizeof(mqd_t));
	int cur_client_id = 0;
	int active_clients = 0;

    struct init_msg init_buffer;
	char log_message[2048];

    while(run || active_clients) 
    {
		if (run) {
			ssize_t bytes_read = mq_receive(mq_server, (char *) &init_buffer, MAX_MSG_SIZE, NULL);
			if(bytes_read != -1)
			{
				mqd_t client_queue = mq_open(init_buffer.IPC_ID, O_RDWR | O_NONBLOCK, 0666, NULL);
				if (client_queue == (mqd_t) -1) {
					perror("Server - klient - mq_open()");
					exit(1);
				}
				clients[cur_client_id] = client_queue;
				active_clients++;
				printf("Klient nr %d dolaczyl! Aktywni: %d\n", cur_client_id, active_clients);
				fflush(stdout);
				sprintf(log_message, "Uzytkownik %d dolaczyl do czatu - %s", cur_client_id, get_time());
				fwrite(log_message, 1, strlen(log_message), logs);
				cur_client_id++;
			}
		}

		for (int i = 0; i < MAX_CLIENTS; i++ ) {
			mqd_t client_queue = clients[i];
			if (client_queue == 0) continue;
			if (!run) {
				struct command_msg halt_msg;
				halt_msg.msg_type = SERVER_STOP;
				mq_send(client_queue, (char *) &halt_msg, MAX_MSG_SIZE, 1);
				active_clients--;
			}
			unsigned int prio = 0;
			struct command_msg command_buffer;
			ssize_t bytes_read = mq_receive(client_queue, (char *) &command_buffer, MAX_MSG_SIZE, &prio);
			if (bytes_read == -1) continue;
			if (prio == 1) {
				struct command_msg response_copy;
				response_copy.msg_type = command_buffer.msg_type;
				response_copy.recipient_id = command_buffer.recipient_id;
				strcpy(response_copy.msg, command_buffer.msg);
				mq_send(client_queue, (char *) &response_copy, MAX_MSG_SIZE, 1);
				continue;
			}
			switch (command_buffer.msg_type) {
				case COMMAND_STOP:
					clients[i] = 0;
					active_clients--;
					printf("Klient nr %d nas opuscil :(. Aktywni: %d\n", i, active_clients);
					fflush(stdout);
					sprintf(log_message, "Uzytkownik %d opuscil czat - %s", i, get_time());
					fwrite(log_message, 1, strlen(log_message), logs);
					break;
				case COMMAND_LIST:
					struct command_msg res;
					res.msg_type = CLIENT_RESPONSE;
					char * list = list_clients(clients);
					strcpy(res.msg, list);
					free(list);
					if (mq_send(client_queue, (char *) &res, sizeof(struct command_msg), 1) == -1) {
						perror("xd");
						exit(1);
					}
					sprintf(log_message, "Uzytkownik %d wywowal LIST - %s", i, get_time());
					fwrite(log_message, 1, strlen(log_message), logs);
					break;
				case COMMAND_2ALL:
					for (int j = 0; j < MAX_CLIENTS; j++) {
						if (clients[j] == 0 || j == i) continue;
						struct command_msg message1; 
						message1.msg_type = CLIENT_RESPONSE;
						char * formatted_message1 = format_message(command_buffer.msg, i);
						strcpy(message1.msg, formatted_message1);
						sprintf(log_message, "Uzytkownik %d wyslal wszystkim wiadomosc o tresci \"%s\" - %s", i, command_buffer.msg, get_time());
						fwrite(log_message, 1, strlen(log_message), logs);
						free(formatted_message1);
						mq_send(clients[j], (char *) &message1, MAX_MSG_SIZE, 1);
					}
					break;
				case COMMAND_2ONE:
					int recepient_queue = clients[command_buffer.recipient_id];
					if (recepient_queue == 0) break;
					struct command_msg message;
					message.msg_type = CLIENT_RESPONSE;
					char * formatted_message = format_message(command_buffer.msg, i);
					strcpy(message.msg, formatted_message);
					sprintf(log_message, "Uzytkownik %d wyslal uzytkownikowi %d wiadomosc o tresci \"%s\" - %s", i, command_buffer.recipient_id, command_buffer.msg, get_time());
						fwrite(log_message, 1, strlen(log_message), logs);
					free(formatted_message);
					mq_send(recepient_queue, (char *) &message, MAX_MSG_SIZE, 1);
					break;
				default:
					break;
			}
		}
		sleep((10.0 - active_clients) / 10.0);
    }

    // Close the message queue
    mq_close(mq_server);
    mq_unlink(QUEUE_NAME);
	free(clients);

    return 0;
}

