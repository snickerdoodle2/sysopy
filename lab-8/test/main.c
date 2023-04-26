#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_t thread01, thread02;

void handle(int signum) {
}

int i;

void * fun_watka(void * cos) {
	while (1) {
		pause();
		printf("%d %s\n", i, (char *) cos);
		fflush(stdout);
	}
}

int main() {
	signal(SIGUSR1, handle);
	pthread_create(&thread01, NULL, &fun_watka, "A");
	pthread_create(&thread02, NULL, &fun_watka, "B");

	for (i = 0; i < 1000; i++) {
		sleep(1);
		pthread_kill(thread01, SIGUSR1);
		pthread_kill(thread02, SIGUSR1);
		fflush(stdout);
	}
}
