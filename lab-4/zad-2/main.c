#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static void au(int sig_no, siginfo_t * info, void * ctx) {
	printf("Otrzymalem sygnal %d, z wartoscia %d, od pid: %d, od uzytkownika %d, o statusie: %d.\n", sig_no, info->si_value.sival_int, info->si_pid, info->si_uid, info->si_code);
}

int main() {
	struct sigaction act;
	act.sa_flags = SA_SIGINFO | SA_INTERRUPT | SA_RESTART;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = au;

	sigaction(SIGUSR1, &act, NULL);
	for (int i = 0; i < 10; i++){
		union sigval sv;
		sv.sival_int = i;

		sigqueue(getpid(), SIGUSR1, sv);
	}

	return 0;
}