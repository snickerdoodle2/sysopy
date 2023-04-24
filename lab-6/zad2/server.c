#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "types.h"

#define MAX_MSG_SIZE 256
#define MAX_MSG_NUM 10

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
    mq_server = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
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
        if(bytes_read != -1)
        {
        	buffer[bytes_read] = '\0';
        	printf("Server received: %s\n", buffer);
        }
    }

    // Close the message queue
    mq_close(mq_server);
    mq_unlink(QUEUE_NAME);

    return 0;
}