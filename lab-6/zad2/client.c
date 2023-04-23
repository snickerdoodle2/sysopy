#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <mqueue.h>

#include "types.h"

#define MAX_MSG_SIZE 256
#define MAX_MSG_NUM 10

int main()
{
	// char * homedir = getenv("Home");
	// key_t server_key = ftok(homedir, 1) % 100;
	// char queue_path[64];
	// sprintf(queue_path, "/test_queue");
    mqd_t mq_server;

    // Open the client message queue
    mq_server = mq_open(QUEUE_NAME, O_WRONLY, 0666, NULL);
    if(mq_server == (mqd_t) -1)
    {
        perror("Client: mq_open()");
        exit(1);
    }

    char buffer[MAX_MSG_SIZE] = "Hello, server!";
    if(mq_send(mq_server, buffer, MAX_MSG_SIZE, 0) == -1)
    {
        perror("Client: mq_send()");
        exit(1);
    }

    // Close the message queue
    mq_close(mq_server);

    return 0;
}
