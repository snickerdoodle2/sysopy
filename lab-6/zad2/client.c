#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <sys/ipc.h>

#include "types.h"

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
	mqd_t mq_client= mq_open(client_server_name, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
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

    // Close the message queue
    mq_close(mq_server);
	mq_close(mq_client);
	mq_unlink(client_server_name);
	free(client_server_name);
    return 0;
}
