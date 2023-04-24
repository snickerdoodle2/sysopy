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
    mqd_t mq_client;
    struct mq_attr attr;

    // Set up the attribute structure
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSG_NUM;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Open the client message queue
    mq_client = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY , 0666, &attr);
    if(mq_client == (mqd_t) -1)
    {
        perror("Client: mq_open()");
        exit(1);
    }

    char buffer[MAX_MSG_SIZE] = "Hello, server!";
    if(mq_send(mq_client, buffer, MAX_MSG_SIZE, 0) == -1)
    {
        perror("Client: mq_send()");
        exit(1);
    }

    // Close the message queue
    mq_close(mq_client);

    return 0;
}