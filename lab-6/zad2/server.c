#include <fctnl.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	char * homedir = getenv("Home");
	ket_t server_key = ftok(homedir, 1);
	char * queue_path[64];
	sprintf(queue_path, "/%d", server_key);

	mqd_t server_queue = mq_open(queue_path, O_CREAT | O_RDWR, 0666, NULL);
	if (server_queue == -1) {
		printf("Nie udalo sie utworzyc kolejki!\n");
		return 1;
	}

	FILE * logs = fopen("logs.txt", "w");
	if (logs == NULL) {
		printf("Nie udalo sie utworzyc pliku!\n");
		return 1;
	}

	fclose(logs);
	mq_unlink(queue_path);
}