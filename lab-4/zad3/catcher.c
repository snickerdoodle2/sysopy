#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int wait = 1;

void handle(int sig_no, siginfo_t * info, void * ctx){
	int sender = info->si_pid;
	int instruction = info->si_value.sival_int;
	printf("Otrzymano od procesu z PID: %d instrukcje nr %d\n", sender, instruction);
	if (instruction == 5) wait = 0;
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
		pause();
	};
	return 0;
}