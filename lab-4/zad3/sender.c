#include <bits/types/sigset_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handle(int sig_no){
	return;
}

int main(int argc, char ** argv) {
	if (argc < 2){
		printf("Za malo argumentow!\n");
	}

	signal(SIGUSR1, handle);

	int pid = atoi(argv[1]);

	for (int i = 2; i < argc; i++){
    	int command = atoi(argv[i]);
		if (command <= 5 && command >= 1) {
			union sigval sv;
			sv.sival_int = command;
			sigqueue(pid, SIGUSR1, sv);
			pause();
        }
	}

	return 0;
}