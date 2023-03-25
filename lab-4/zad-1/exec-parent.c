#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

void handle_signal(int sig_code) {
	printf("Otrzymano sygnał: %d\n", sig_code);
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("Więcej argumentów!\n");
		return 1;
	}

	char * command = argv[1];

	sigset_t newmask, oldmask;

	if (strcmp(command, "ignore") == 0){
		signal(SIGUSR1, SIG_IGN);
	} else if (strcmp(command, "handler") == 0){
		signal(SIGUSR1, handle_signal);
	} else if (strcmp(command, "mask") == 0 || strcmp(command, "pending") == 0){
		sigemptyset(&newmask);
		sigaddset(&newmask, SIGUSR1);
		int res = sigprocmask(SIG_BLOCK, &newmask, &oldmask);
		if (res < 0) {
			printf("Nie udało się zablokować sygnału\n");
			return 1;
		}

	} else {
		printf("Nie znaleliozno komendy!\n");
		return 1;
	}

	raise(SIGUSR1);

	execl("./exec-child", "./exec-child", command, NULL);	

	return 0;
}