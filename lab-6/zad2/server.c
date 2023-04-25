#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "types.h"

int run = 1;

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

	mqd_t * clients = calloc(MAX_CLIENTS, sizeof(mqd_t));
	int cur_client_id = 0;
	int active_clients = 0;

    struct init_msg init_buffer;


    while(run)
    {
		if (run) {
			ssize_t bytes_read = mq_receive(mq_server, (char *) &init_buffer, MAX_MSG_SIZE, NULL);
			if(bytes_read != -1)
			{
				mqd_t client_queue = mq_open(QUEUE_NAME, O_WRONLY , 0666, NULL);
				if (client_queue == (mqd_t) -1) {
					perror("Server - klient - mq_open()");
					exit(1);
				}
				clients[cur_client_id] = client_queue;
				active_clients++;
				printf("Klient nr %d dolaczyl! Aktywni: %d\n", cur_client_id, active_clients);
				fflush(stdout);
				cur_client_id++;
			}
		}
    }

    // Close the message queue
    mq_close(mq_server);
    mq_unlink(QUEUE_NAME);
	free(clients);

    return 0;
}
