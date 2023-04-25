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
int run = 1;

void handle(int signum) {
	if (pid != 0) {
		kill(pid, SIGINT);
		struct command_msg command;
		command.msg_type = COMMAND_STOP;
		(void) command;
		// send halt 
		sleep(1);
		run = 0;
	} else {
		exit(0);
	}
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

	char * client_server_name = malloc(64);
	char * homedir = getenv("HOME");
	key_t client_key = ftok(homedir, getpid());
	sprintf(client_server_name, "/%d", client_key);

	// Create the server message queue
	mq_unlink(client_server_name);
	mq_client = mq_open(client_server_name, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
	if (mq_client == (mqd_t) -1) {
		perror("Client: mq_open()");
		exit(1);
	}

	struct init_msg init_msg;
	init_msg.msg_type = CLIENT_INIT;
	strcpy(init_msg.IPC_ID, client_server_name);

	if(mq_send(mq_server, (char *) &init_msg, sizeof(struct init_msg), 0) == -1)
	{
		perror("Client: mq_send()");
		exit(1);
	}

	signal(SIGINT, handle);

	pid = fork();

	if (pid != 0){
		struct response_msg response_buffer;
		while (run) {
			ssize_t bytes_read = mq_receive(mq_client, (char *) &response_buffer, MAX_MSG_SIZE, NULL);
			if (bytes_read > 0) {
				if (response_buffer.msg_type == SERVER_STOP) {
					kill(getppid(), SIGINT);
				} else {
					printf("%s", response_buffer.res);
					fflush(stdout);
				}
			}
		} 
	} else { 
		char command_buffer[1024];
		while(run && fgets(command_buffer, 1024, stdin)) {
			char * command = strtok(command_buffer, " ");
			struct command_msg command_msg;
			printf("%s", command);
			fflush(stdout);
			int send = 1;
			if (strcmp(command, "LIST\n") == 0) {
				command_msg.msg_type = COMMAND_LIST;
			} else if (strcmp(command, "2ALL") == 0) {
				command_msg.msg_type = COMMAND_2ALL;
				char * message_buff = strtok(NULL, "");
				if (message_buff != NULL) {
					strcpy(command_msg.msg, message_buff);
				}
			} else if (strcmp(command, "2ONE") == 0) {
				command_msg.msg_type = COMMAND_2ONE;
				command_msg.recipient_id = atoi(strtok(NULL, " "));
				command_msg.msg_type = COMMAND_2ONE;
				if (message_buff != NULL) {
					strcpy(command_msg.msg, message_buff);
				}
			} else if (strcmp(command, "STOP") == 0) {
				send = 0;
				raise(SIGINT);
			} else {
				send = 0;
				printf("Nieznana komenda\n");
				fflush(stdout);
			}
			if (send) {
				if(mq_send(mq_client, (char *) &command_msg, sizeof(struct command_msg), 0) == -1) {
					perror("Client: mq_send()");
					exit(1);
				}
			}
		}
	}

	mq_close(mq_server);
	mq_close(mq_client);
	mq_unlink(client_server_name);
	free(client_server_name);
	return 0;
}
