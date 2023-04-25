#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>

#include "types.h"

int pid;
mqd_t mq_client;
char client_server_name[64];
int run = 1;

void handle(int signum) {
	if (pid != 0) {
		kill(pid, SIGINT);
		struct command_msg command_message;
		command_message.msg_type = COMMAND_STOP;
		if (mq_send(mq_client, (char *) &command_message, sizeof(struct command_msg), 0) == -1) {
			perror("xdd");
			exit(1);
		}
		sleep(2);
		mq_close(mq_client);
		mq_unlink(client_server_name);
	}
	exit(0);
}



int main()
{
	mqd_t mq_server = mq_open(QUEUE_NAME, O_WRONLY , 0666, NULL);
	if(mq_server == (mqd_t) -1)
	{
		perror("Client: mq_open()");
		exit(1);
	}
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MSG_NUM;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	char * homedir = getenv("HOME");
	key_t client_key = ftok(homedir, getpid());
	sprintf(client_server_name, "/%d", client_key);

	// Create the server message queue
	mq_unlink(client_server_name);
	mq_client = mq_open(client_server_name, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);
	if (mq_client == (mqd_t) -1) {
		perror("Client: mq_open()");
		exit(1);
	}

	signal(SIGINT, handle);

	struct init_msg init_msg;
	init_msg.msg_type = CLIENT_INIT;
	strcpy(init_msg.IPC_ID, client_server_name);

	if(mq_send(mq_server, (char *) &init_msg, sizeof(struct init_msg), 0) == -1)
	{
		perror("Client: mq_send()");
		exit(1);
	}

	mq_close(mq_server);

	pid = fork();

	if (pid == 0) {
		struct command_msg response;
		unsigned int prio;
		while (1) {
			ssize_t bytes_read = mq_receive(mq_client, (char *) &response, MAX_MSG_SIZE, &prio);
			if (bytes_read != -1) {
				if (prio == 0) {
					struct command_msg response_copy;
					response_copy.msg_type = response.msg_type;
					response_copy.recipient_id = response.recipient_id;
					strcpy(response_copy.msg, response.msg);
					mq_send(mq_client, (char *) &response_copy, MAX_MSG_SIZE, 0);
				} else {
					if (response.msg_type == SERVER_STOP) kill(getppid(), SIGINT);
					else {
						printf("%s", response.msg);
						fflush(stdout);
					}
				}
				}
			}
	} else {
		char command_buffer[1024];
		while(fgets(command_buffer, 1024, stdin)) {
			char * command_name = strtok(command_buffer, " ");
			struct command_msg command_message;
			int send = 1;

			if (strcmp(command_name, "LIST\n") == 0) {
				command_message.msg_type = COMMAND_LIST;
			} else if (strcmp(command_name, "2ALL") == 0) {
				command_message.msg_type = COMMAND_2ALL;
				char * message = strtok(NULL, "");
				if (message != NULL) {
					strcpy(command_message.msg, message);
				}
			} else if (strcmp(command_name, "2ONE") == 0) {
				command_message.msg_type = COMMAND_2ONE;
				command_message.recipient_id = atoi(strtok(NULL, " "));
				char * message = strtok(NULL, "");
				if (message != NULL) {
					strcpy(command_message.msg, message);
				}
			} else if (strcmp(command_name, "STOP\n") == 0) {
				raise(SIGINT);
				send = 0;
			} else {
				printf("Nieznana komenda!\n");
				send = 0;
			}


			if (send) {
				if (mq_send(mq_client, (char *) &command_message, sizeof(struct command_msg), 0) == -1) {
					perror("xdd");
					exit(1);
				}
			}
		}
	}
	return 0;
}
