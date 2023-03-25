#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int wait = 1;
int show_time = 0;
int count = 0;

void print_cur_time() {
	time_t now = time(0);
	printf("%s", asctime(gmtime(&now)));
}

void handle(int sig_no, siginfo_t * info, void * ctx){
	int sender = info->si_pid;
	int instruction = info->si_value.sival_int;
	show_time = 0;
	++count;
	switch (instruction) {
		case 1:
			for (int i = 1; i <= 100; i++){
				printf("%d\n", i);
            }
			break;
		case 2:
			print_cur_time();
			break;
		case 3:
			printf("%d\n", count);
			break;
		case 4:
			show_time = 1;
			break;
		case 5:
			wait = 0;
			break;
		default:
			break;
	}


	kill(sender, SIGUSR1);
}

int main() {
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = handle;
	sigaction(SIGUSR1, &act, NULL);

	printf("PID: %d\n", getpid());

	while(wait){
		if (show_time) print_cur_time();
		sleep(1);
	};
	return 0;
}