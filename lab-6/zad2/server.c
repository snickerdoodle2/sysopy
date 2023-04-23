#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include "types.h"
#include <mqueue.h>

#define MAX_MSG_SIZE 256
#define MAX_MSG_NUM 10

int main()
{
	// char * homedir = getenv("Home");
	// key_t server_key = ftok(homedir, 1) % 100;
	// char queue_path[64];
	// sprintf(queue_path, "/testqueue");
	
    mqd_t mq_server;

    // Create the server message queue
    mq_server = mq_open(QUEUE_NAME, O_CREAT | O_NONBLOCK, 0666, NULL);
    if(mq_server == (mqd_t) -1)
    {
        perror("Server: mq_open()");
        exit(1);
    }

    char buffer[MAX_MSG_SIZE];
    unsigned int prio;

    while(1)
    {
        ssize_t bytes_read = mq_receive(mq_server, buffer, MAX_MSG_SIZE, &prio);
		if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Server received: %s\n", buffer);
		}
    }

    // Close the message queue
    mq_close(mq_server);
    mq_unlink(QUEUE_NAME);

    return 0;
}
